// This sketch provide the functionality of OTA Firmware Upgrade
#include <WiFi.h>
#include <HttpsOTAUpdate.h>
#include <MQTT.h>
// This sketch shows how to implement HTTPS firmware update Over The Air.
// Please provide your WiFi credentials, https URL to the firmware image and the server certificate.

static const char* ssid = "Guest56294";         // your network SSID (name of wifi network)
static const char* password = "8977038737619";  // your network password
// const char* HOST = "192.168.24.74";
const char* HOST = "hidden-sea-35212.herokuapp.com";
// static const char* url = "https://192.168.24.74:443/bin/firmware.bin";  //state url of your firmware image
static const char* url = "https://hidden-sea-35212.herokuapp.com/bin/firmware.bin";  //state url of your firmware image

static const char* server_certificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIGZjCCBU6gAwIBAgIQDVfRCxEpKjxm383POui+ZTANBgkqhkiG9w0BAQsFADBw\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMS8wLQYDVQQDEyZEaWdpQ2VydCBTSEEyIEhpZ2ggQXNz\n"
    "dXJhbmNlIFNlcnZlciBDQTAeFw0yMDA2MTUwMDAwMDBaFw0yMTA3MDcxMjAwMDBa\n"
    "MGsxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1T\n"
    "YW4gRnJhbmNpc2NvMRUwEwYDVQQKEwxIZXJva3UsIEluYy4xGDAWBgNVBAMMDyou\n"
    "aGVyb2t1YXBwLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMnN\n"
    "1rH4ObYuY5NHb+xV4VV30Z+dxpkScWFWyi4yN86eTOA99A9FduC00UDgtMwaDD/r\n"
    "6LVftlaY0ymKKa4+O24PEH5MrqR919yJNS2i+llJ0xQHthlKsW9GHuyyeOi6ZpK7\n"
    "cPGuatR+Z5gwjGxPoMEyt1szMX8BUZpsH+/9SJ2zlC0pTm3P06QOZbkQEziQwtOW\n"
    "Pj7285ZZUGHCcfb/1dYjJpxUism8p9TAIZnW6UgZ487r612HFJYFfS3bCHnS4Jsk\n"
    "TsSUg7AuJz7Px46AAKuGJ8CoV9A1GHAq0ngFwph7i6tQIgAxSsniXHz94/JgZimj\n"
    "5rn10dbXU28TjpXQdYECAwEAAaOCAv8wggL7MB8GA1UdIwQYMBaAFFFo/5CvAgd1\n"
    "PMzZZWRiohK4WXI7MB0GA1UdDgQWBBSL9RPT/PsKraCvn2/CQsTYR0zL5TApBgNV\n"
    "HREEIjAggg8qLmhlcm9rdWFwcC5jb22CDWhlcm9rdWFwcC5jb20wDgYDVR0PAQH/\n"
    "BAQDAgWgMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjB1BgNVHR8EbjBs\n"
    "MDSgMqAwhi5odHRwOi8vY3JsMy5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXIt\n"
    "ZzYuY3JsMDSgMqAwhi5odHRwOi8vY3JsNC5kaWdpY2VydC5jb20vc2hhMi1oYS1z\n"
    "ZXJ2ZXItZzYuY3JsMEwGA1UdIARFMEMwNwYJYIZIAYb9bAEBMCowKAYIKwYBBQUH\n"
    "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCAYGZ4EMAQICMIGDBggr\n"
    "BgEFBQcBAQR3MHUwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNv\n"
    "bTBNBggrBgEFBQcwAoZBaHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lD\n"
    "ZXJ0U0hBMkhpZ2hBc3N1cmFuY2VTZXJ2ZXJDQS5jcnQwDAYDVR0TAQH/BAIwADCC\n"
    "AQQGCisGAQQB1nkCBAIEgfUEgfIA8AB2APZclC/RdzAiFFQYCDCUVo7jTRMZM7/f\n"
    "DC8gC8xO8WTjAAABcrkU+VkAAAQDAEcwRQIgOYrojFSS9pLZWvDxx8LcPfs7G1Tq\n"
    "3BhrHClAfLoasOMCIQCPeT5wI6V4TGxmLMLIS1fXXt1KevyGWJ7s08jyHRo3kQB2\n"
    "AFzcQ5L+5qtFRLFemtRW5hA3+9X6R9yhc5SyXub2xw7KAAABcrkU+XcAAAQDAEcw\n"
    "RQIhALZG0NcmoBZwoTSxRTBzs2QanNJVCuKLFek0l3qxfuY3AiBMPdWgBP/6oyFD\n"
    "eIKMbI9DwpJSj0SSJwRVZUgoiHLoJzANBgkqhkiG9w0BAQsFAAOCAQEAaRSAbyG8\n"
    "kJqnb4l8gjFFAOojEZgFeqmaZHJQlsKLfxq6DcNvXmwDGQ/xDmi1GnzjE5wAsjlT\n"
    "bfAIxUwsPt0uJiMzw9lONcTAWRHd2c1CPrsz/pZivGRo/O560MAotgBta057W4zJ\n"
    "YUQiqVPUjjhg/5sM5K1ATR4VDqvV1NEWUlMoY1rQogzlFp1z56L4E1KPnXByI9mh\n"
    "igFuhOsnqa9EkDgQHmBl0YX07XJeGtgvNyvGTmamLkBLdB51ERvd0di6f2piWvOZ\n"
    "VElwmiwO/IhWSiotO6ZYnYXw2qUyDOeHSKHAXZPqGPoB94bGZB3OqbxYn2DkFiZx\n"
    "FVgTiv7/jzinqg==\n"
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
