#include "uWS.h"
#include "event-loop.hpp"
#include "bytearray.hpp"
#include "uv.h"
#include "gc.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

uWS::Group<uWS::CLIENT> *madwebsocketclient__connect(char *url) {
    uWS::Hub *hub = new uWS::Hub(0, false, 16777216, (uS::Loop *) getLoop());
    uWS::Group<uWS::CLIENT> *hubGroup = hub->createGroup<uWS::CLIENT>();

    auto handle = hub->connect(std::string(url), nullptr, { }, 5000, hubGroup);

    hubGroup->tcpHandle = handle;
    

    return hubGroup;
}


void madwebsocketclient__cancelConnection(uWS::Group<uWS::CLIENT> *ws) {
  auto handle = (uv_handle_t *) ws->tcpHandle;

  if (ws->fd > -1) {
    close(ws->fd);
  }

  if (handle && !uv_is_closing(handle)) {
    uv_close(handle, [](auto _){});
  }
}


void madwebsocketclient__onConnected(PAP_t *cb, uWS::Group<uWS::CLIENT> *ws) {
  ws->onConnection([cb](auto _, auto __) {
    __applyPAP__(cb, 1, NULL);
  });
}

void madwebsocketclient__onDisconnected(PAP_t *cb, uWS::Group<uWS::CLIENT> *ws) {
  ws->onDisconnection([cb](auto _, auto __, auto ___, auto ____) {
    __applyPAP__(cb, 1, NULL);
  });
}


void madwebsocketclient__onMessage(PAP_t *cb, uWS::Group<uWS::CLIENT> *ws) {
  ws->onMessage([cb, ws](auto _ws, char *message, size_t length, uWS::OpCode opCode) {
    madlib__bytearray__ByteArray_t *result = (madlib__bytearray__ByteArray_t*) GC_MALLOC(sizeof(madlib__bytearray__ByteArray_t));
    result->bytes = (unsigned char *) GC_MALLOC_ATOMIC(length);
    result->length = length;

    memcpy(result->bytes, message, length);
    __applyPAP__(cb, 1, result);
  });
}


void madwebsocketclient__onError(PAP_t *cb, uWS::Group<uWS::CLIENT> *ws) {
  ws->onError([cb](auto error) {
    __applyPAP__(cb, 1, NULL);
  });
}


void madwebsocketclient__send(madlib__bytearray__ByteArray_t *data, uWS::Group<uWS::CLIENT> *group) {
  group->forEach([data](uWS::WebSocket<false> *ws) {
    ws->send((char *)data->bytes, data->length, uWS::OpCode::BINARY);
  });
}


void madwebsocketclient__disconnect(uWS::Group<uWS::CLIENT> *group) {
  group->terminate();
}


#ifdef __cplusplus
}
#endif
