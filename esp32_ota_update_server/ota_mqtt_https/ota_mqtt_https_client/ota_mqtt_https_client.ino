// This sketch provide the functionality of OTA Firmware Upgrade
#include <WiFi.h>
#include <HttpsOTAUpdate.h>
#include <MQTT.h>
// This sketch shows how to implement HTTPS firmware update Over The Air.
// Please provide your WiFi credentials, https URL to the firmware image and the server certificate.

static const char* ssid = "your-ssid";          // your network SSID (name of wifi network)
static const char* password = "your-password";  // your network password
const char* HOST = "192.168.24.74";
static const char* url = "https://192.168.24.74:443/bin/firmware.bin";  //state url of your firmware image

static const char* server_certificate =
    "-----BEGIN CERTIFICATE-----\n"
    "-----END CERTIFICATE-----";

static HttpsOTAStatus_t otastatus;

void HttpEvent(HttpEvent_t* event) {
    switch (event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

WiFiClient net;
MQTTClient client;

void connect() {
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

    // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
    // by Arduino. You need to set the IP address directly.
    // client.begin("public.cloud.shiftr.io", net);
    client.begin(HOST, 1883, net);
    client.onMessage(messageReceived);

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

        // update firmware from https server
        HttpsOTA.onHttpEvent(HttpEvent);
        Serial.println("Starting OTA");
        HttpsOTA.begin(url, server_certificate);

        while (true) {
            otastatus = HttpsOTA.status();
            if (otastatus == HTTPS_OTA_SUCCESS) {
                Serial.println("Firmware written successfully");
                if (payload == "now") {
                    Serial.println("Auto Restart in 3 seconds");
                    delay(3000);
                    ESP.restart();
                } else {
                    Serial.println("To reboot device, PUSH restart button on device");
                }
                break;
            } else if (otastatus == HTTPS_OTA_FAIL) {
                Serial.println("Firmware Upgrade Fail");
                break;
            } else if (otastatus == HTTPS_OTA_ERR) {
                Serial.println("Firmware Upgrade Error");
                break;
            } else if (otastatus == HTTPS_OTA_UPDATING) {
                Serial.print(".");
            } else {  // HTTPS_OTA_IDLE
                ;
            }
            delay(1000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.print("Attempting to connect to SSID: ");
    WiFi.begin(ssid, password);

    connect();
}

void loop() {
    client.loop();
    delay(10);  // <- fixes some issues with WiFi stability

    if (!client.connected()) {
        connect();
    }
}
