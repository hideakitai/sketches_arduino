Because general cloud server cannot use MQTT port,
and also MQTT over WS/WSS uses HTTP/HTTPS port,
I use HTTP API available on shiftr.

- broker: shiftr.io
- file-server (MQTT client): heroku
- device <- MQTTS -> broker (currently this is MQTT...)
- device <- HTTPS -> server (currently this is HTTP...)
- broker <- HTTP API (shiftr) -> server
