#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

AutoConnect portal;

void setup() {
    Serial.begin(115200);
    delay(2000);

    // required to see what the ssid and ip are
    portal.onConnect([](const IPAddress& ip) {
        Serial.print("SSID : ");
        Serial.println(WiFi.SSID());
        Serial.print("IP   : ");
        Serial.println(ip);
    });

    AutoConnectConfig config;
    config.ota = AC_OTA_BUILTIN;  // required to show ota page
    portal.config(config);
    portal.begin();  // no need to set ssid and pwd
    // or we can use them manually
    // portal.begin(ssid, pwd);

    // you can access "http://[device_ip]/_ac" for homepage
    // or register the root page redirector to "/_ac"
    // WebServer& server = portal.host();
    // server.on("/", [&]() {
    //     const String ip = server.client().localIP().toString();
    //     server.sendHeader("Location", String("http://") + ip + String("/_ac"));
    //     server.send(302, "text/plain", "");
    //     server.client().flush();
    //     server.client().stop();
    // });
}

void loop() {
    portal.handleClient();
}
