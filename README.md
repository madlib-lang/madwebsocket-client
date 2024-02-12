# madwebsocket-client

## API

```madlib
connect :: String -> WebSocket
onConnected :: ({} -> {}) -> WebSocket -> {}
onDisconnected :: ({} -> {}) -> WebSocket -> {}
onError :: (WebSocketError -> {}) -> WebSocket -> {}
onMessage :: (ByteArray -> {}) -> WebSocket -> {}
send :: ByteArray -> WebSocket -> {}
```

## Example

```madlib
import ByteArray from "ByteArray"
import IO from "IO"
import Wish from "Wish"

import WebSocket from "./Main"



noop = () => {}

delaySend :: Integer -> ByteArray -> WebSocket -> {}
delaySend = (after, data, ws) => {
  Wish.fulfill(
    noop,
    noop,
    do {
      _ <- Wish.after(after, {})
      WebSocket.send(data, ws)
      return of({})
    },
  )

  return {}
}

main = () => {
  ws = WebSocket.connect("ws:/localhost:3000/path")

  WebSocket.onError(
    pipe(
      .code,
      IO.putLine,
    ),
    ws,
  )

  WebSocket.onConnected(
    () => {
      WebSocket.send(ByteArray.fromString("HELLO"), ws)
      IO.putLine("OPENED")
    },
    ws,
  )

  WebSocket.onDisconnected(() => { IO.putLine("CLOSED") }, ws)

  WebSocket.onMessage(
    (msg) => {
      IO.putLine(ByteArray.toString(msg))
      delaySend(1000, msg, ws)
    },
    ws,
  )
}

```
