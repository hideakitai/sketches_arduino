#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "your-ssid";
const char pass[] = "your-pass";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("\nconnecting...");
    // while (!client.connect("my_client_id")) {  // test.mosquitto.org or other anonymous connection
    // while (!client.connect("my_client_id", "public", "public")) {  // public.shiftr.io
    while (!client.connect("my_client_id", "instance_name", "secret_token")) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    client.subscribe("/hello");
    // client.unsubscribe("/hello");
}

void messageReceived(String& topic, String& payload) {
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);

    // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
    // by Arduino. You need to set the IP address directly.
    // client.begin("test.mosquitto.org", net);  // mosquitto
    // client.begin("public.cloud.shiftr.io", net);  // shiftr
    client.begin("instance_name.cloud.shiftr.io", net);  // my shiftr instance
    client.onMessage(messageReceived);

    connect();
}

void loop() {
    client.loop();
    delay(10);  // <- fixes some issues with WiFi stability

    if (!client.connected()) {
        connect();
    }

    // publish a message roughly every second.
    if (millis() - lastMillis > 1000) {
        lastMillis = millis();
        client.publish("/hello", "world");
    }
}