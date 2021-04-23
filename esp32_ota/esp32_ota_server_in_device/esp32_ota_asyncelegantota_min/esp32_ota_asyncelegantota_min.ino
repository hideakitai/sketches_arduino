#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = "your-ssid";
const char* pwd = "your-pwd";

AsyncWebServer server(80);

void setup(void) {
    Serial.begin(115200);
    delay(2000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("SSID : ");
    Serial.println(ssid);
    Serial.print("IP   : ");
    Serial.println(WiFi.localIP());

    // we can access to ota page at "http://[device_ip]/update"
    AsyncElegantOTA.begin(&server);
    server.begin();
}

void loop(void) {
    AsyncElegantOTA.loop();
}
