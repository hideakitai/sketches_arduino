#include "certs.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

// Wifi credentials
const char* SSID = "your-ssid";
const char* PASSWORD = "your-pass";
const IPAddress ip(192, 168, 0, 201);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

// The name of the device. This MUST match up with the name defined in the AWS console
#define DEVICE_NAME "MyEsp32"
// The MQTTT endpoint for the device (unique for each AWS account but shared amongst devices within the account)
#define AWS_IOT_ENDPOINT "your-endpoint.iot.ap-somewhre.amazonaws.com"
// The MQTT topic that this device should publish to
#define AWS_IOT_TOPIC "MyEsp32"
// How many times we should attempt to connect to AWS
#define AWS_MAX_RECONNECT_TRIES 50

// MQTT
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    // Only try 15 times to connect to the WiFi
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 15) {
        delay(500);
        Serial.print(".");
        retries++;
    }

    // If we still couldn't connect to the WiFi, go to deep sleep for a minute and try again.
    if (WiFi.status() != WL_CONNECTED) {
        esp_sleep_enable_timer_wakeup(1 * 60L * 1000000L);
        esp_deep_sleep_start();
    }

    Serial.print("SSID : ");
    Serial.println(SSID);
    Serial.print("IP   : ");
    Serial.println(WiFi.localIP());
}

void connectToAWS() {
    // Configure WiFiClientSecure to use the AWS certificates we generated
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Try to connect to AWS and count how many times we retried.
    int retries = 0;
    Serial.print("Connecting to AWS IOT: ");

    while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES) {
        Serial.print(".");
        delay(100);
        retries++;
    }

    // Make sure that we did indeed successfully connect to the MQTT broker
    // If not we just end the function and wait for the next loop.
    if (!client.connected()) {
        Serial.println(" Timeout!");
        return;
    }

    // If we land here, we have successfully connected to AWS!
    // And we can subscribe to topics and send messages.
    Serial.println("Connected!");
    Serial.print("My prefix is: ");
    Serial.println(AWS_IOT_TOPIC);
}

void sendJsonToAWS() {
    StaticJsonDocument<128> jsonDoc;
    JsonObject stateObj = jsonDoc.createNestedObject("state");
    JsonObject reportedObj = stateObj.createNestedObject("reported");

    // Write the temperature & humidity. Here you can use any C++ type (and you can refer to variables)
    reportedObj["temperature"] = 23.76;
    reportedObj["humidity"] = 78.12;
    reportedObj["wifi_strength"] = WiFi.RSSI();

    // Create a nested object "location"
    JsonObject locationObj = reportedObj.createNestedObject("location");
    locationObj["name"] = "Garden";

    Serial.println("Publishing message to AWS...");
    //serializeJson(doc, Serial);
    char jsonBuffer[512];
    serializeJson(jsonDoc, jsonBuffer);

    client.publish(AWS_IOT_TOPIC, jsonBuffer);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    connectToWiFi();
    connectToAWS();
}

void loop() {
    sendJsonToAWS();
    client.loop();
    delay(5000);
}
