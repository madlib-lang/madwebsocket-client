#include "event-loop.hpp"
#include "gc.h"
#include "record.hpp"
#include <thread>
#include "uv.h"
#include "apply-pap.hpp"
#include "http.hpp"
#include "list.hpp"
#include "record.hpp"
#include "maybe.hpp"
#include "bytearray.hpp"
#include <iostream>
#include "uWS.h"

int main()
{
  uv_loop_t *_loop = (uv_loop_t *)GC_MALLOC_UNCOLLECTABLE(sizeof(uv_loop_t));
  uv_loop_init(_loop);
  // uWS::Loop::get(_loop);
  // uWS::ClientApp *client = new uWS::ClientApp();
  // client->connect("localhost", 3000, _loop, "");

  uWS::Hub hub(0, false, 16777216, (uS::Loop *)_loop);
  uWS::Group<uWS::CLIENT> *hubGroup;
  std::unique_ptr<uS::Async> hubTrigger;

  hubGroup = hub.createGroup<uWS::CLIENT>();

  hubGroup->onConnection([](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req)
                         {
        std::cout << "connect, sending HELLO" << std::endl;
        std::string msg = "HELLO";
        ws->send(msg.data(), msg.size(), uWS::OpCode::TEXT); });

  hubGroup->onMessage([](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode)
                      {
        std::cout << "Got reply: " << std::string(message, length) << std::endl;
        ws->terminate(); });

  hubGroup->onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length)
                            { std::cout << "Disconnect." << std::endl; });

  hub.connect("ws://localhost:3000", nullptr, {}, 5000, hubGroup);

  // hub.run();
  uv_run(_loop, UV_RUN_DEFAULT);
}
