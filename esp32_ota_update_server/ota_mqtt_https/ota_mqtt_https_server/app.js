require("log-timestamp");

// MQTT broker
const aedes = require("aedes")();
const server = require("net").createServer(aedes.handle);
const port = 1883;
server.listen(port, function () {
  console.log("server started and listening on port ", port);
});

// fired when topic is subscribed
aedes.on("subscribe", function (subscriptions, client) {
  console.log(
    "MQTT client \x1b[32m" +
      (client ? client.id : client) +
      "\x1b[0m subscribed to topics: " +
      subscriptions.map((s) => s.topic).join("\n"),
    "from broker",
    aedes.id
  );
});

// fired when topic is unsubscribed
aedes.on("unsubscribe", function (subscriptions, client) {
  console.log(
    "MQTT client \x1b[32m" +
      (client ? client.id : client) +
      "\x1b[0m unsubscribed to topics: " +
      subscriptions.join("\n"),
    "from broker",
    aedes.id
  );
});

// fired when a client connects
aedes.on("client", function (client) {
  console.log(
    "Client Connected: \x1b[33m" + (client ? client.id : client) + "\x1b[0m",
    "to broker",
    aedes.id
  );
});

// fired when a client disconnects
aedes.on("clientDisconnect", function (client) {
  console.log(
    "Client Disconnected: \x1b[31m" + (client ? client.id : client) + "\x1b[0m",
    "to broker",
    aedes.id
  );
});

// fired when a message is published
aedes.on("publish", async function (packet, client) {
  console.log(
    "Client \x1b[31m" +
      (client ? client.id : "BROKER_" + aedes.id) +
      "\x1b[0m has published",
    packet.payload.toString(),
    "on",
    packet.topic,
    "to broker",
    aedes.id
  );
});

// HTTPS Server for firmware download
const express = require("express");
const https = require("https");
const fs = require("fs");
const HTTPS_PORT = 443;
const app = express();
// send firmware.bin if requested
app.get("/bin/firmware.bin", (_, res) => res.download("./bin/firmware.bin"));
https
  .createServer(
    {
      key: fs.readFileSync("./key.pem"),
      cert: fs.readFileSync("./cert.pem"),
      // passphrase: "your-passphrase",
    },
    app
  )
  .listen(HTTPS_PORT);

// file watcher
const chokidar = require("chokidar");
chokidar.watch("./bin/firmware.bin").on("change", (path) => {
  console.log(path);
  aedes.publish({ topic: "ota/update", payload: "now" });
});
