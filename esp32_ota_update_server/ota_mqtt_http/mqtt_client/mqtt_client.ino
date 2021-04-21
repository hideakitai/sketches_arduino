#include <WiFi.h>
#include <MQTT.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

const char ssid[] = "your-ssid";
const char pass[] = "your-password";
const char* HOST = "192.168.0.17";

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
    // while (!client.connect("arduino", "public", "public")) {
    while (!client.connect("arduino")) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    client.subscribe("ota/update");
}

void messageReceived(String& topic, String& payload) {
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.

    if (topic == "ota/update") {
        Serial.println("update requested");
        // update firmware from http server
        WiFiClient client;
        // t_httpUpdate_return ret = httpUpdate.update(client, HOST, 8080, "/bin/firmware.bin");
        // OR
        t_httpUpdate_return ret = httpUpdate.update(client, "http://192.168.0.17:8080/bin/firmware.bin");

        switch (ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                break;
            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;
            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.disconnect(true, true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
    // by Arduino. You need to set the IP address directly.
    // client.begin("public.cloud.shiftr.io", net);
    client.begin(HOST, 1883, net);
    client.onMessage(messageReceived);

    connect();
}

void loop() {
    client.loop();
    delay(10);  // <- fixes some issues with WiFi stability

    if (!client.connected()) {
        connect();
    }
}
