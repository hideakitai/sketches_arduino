// Because general cloud server cannot use MQTT port,
// and also MQTT over WS/WSS uses HTTP/HTTPS port,
// I use HTTP API available on shiftr.
//
// broker: shiftr.io
// file-server (MQTT client): heroku
// device <- MQTTS -> broker
// device <- HTTPS -> server
// broker <- HTTP API (shiftr) -> server

require("log-timestamp");

const axios = require("axios");

// HTTP Server for firmware download
const express = require("express");
const http = require("http");
// const https = require("https");
// const HTTP_PORT = 443;
// const fs = require("fs");
const app = express();
app.get("/", function (_, res) {
  res.send("hello world");
});
// send firmware.bin if requested
app.get("/bin/firmware.bin", (_, res) => res.download("./bin/firmware.bin"));
http.createServer(app).listen(process.env.PORT);
// https
//   .createServer(
//     {
//       key: fs.readFileSync("./key.pem"),
//       cert: fs.readFileSync("./cert.pem"),
//       // passphrase: "your-passphrase",
//     },
//     app
//   )
//   .listen(HTTP_PORT);

// shiftr http interface
// curl -X POST 'ENDPOINT/broker/foo/bar' -d 'Hello World!'
// post ota/update if this app is deployed (maybe it is when firmware is updated)
function post_update() {
  axios
    .post(
      "https://INSTANCE_NAME:TOKEN_SECRET@INSTANCE_DOMAIN/broker/ota/update",
      "now"
    )
    .then(function (response) {
      console.log(response);
    })
    .catch(function (error) {
      console.log(error);
    });
}

setTimeout(post_update, 5000);

// // file watcher
// const chokidar = require("chokidar");
// chokidar.watch("./bin/firmware.bin").on("change", (path) => {
//   console.log(path);

//   // shiftr http interface
//   // curl -X POST 'ENDPOINT/broker/foo/bar' -d 'Hello World!'
//   axios
//     .post(
//       "https://INSTANCE_NAME:TOKEN_SECRET@INSTANCE_DOMAIN/broker/ota/update",
//       "now"
//     )
//     .then(function (response) {
//       console.log(response);
//     })
//     .catch(function (error) {
//       console.log(error);
//     });
// });
