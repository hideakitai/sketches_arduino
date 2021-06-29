#include <ArduinoJson.h>

void setup() {
    Serial.begin(115200);

    // StaticJsonObject allocates memory on the stack, it can be
    // replaced by DynamicJsonObject which allocates in the heap.
    //
    // DynamicJsonObject doc(200);

    // MessagePack input string.
    //
    // Using a char[], as shown here, enables the "zero-copy" mode. This mode uses
    // the minimal amount of memory because the JsonDocument stores pointers to
    // the input buffer.
    // If you use another type of input, ArduinoJson must copy the strings from
    // the input to the JsonDocument, so you need to increase the capacity of the
    // JsonDocument.
    // uint8_t input[] = {131, 166, 115, 101, 110, 115, 111, 114, 163, 103, 112, 115,
    //     164, 116, 105, 109, 101, 206, 80, 147, 50, 248, 164, 100,
    //     97, 116, 97, 146, 203, 64, 72, 96, 199, 58, 188, 148,
    //     112, 203, 64, 2, 106, 146, 230, 33, 49, 169};
    // This MessagePack document contains:
    // {
    //   "sensor": "gps",
    //   "time": 1351824120,
    //   "data": [48.75608, 2.302038]
    // }

    // StaticJsonDocument<200> doc;
    DynamicJsonDocument doc(200);

    doc["sensor"] = "gps";
    doc["time"] = 1351824120;
    JsonArray data = doc.createNestedArray("data");
    data.add(48.756080);
    data.add(2.302038);

    Serial.print("json memory pool size = ");
    Serial.println(doc.memoryUsage());

    Serial.print("json document length = ");
    Serial.println(measureJson(doc));

    Serial.print("msgpack length = ");
    Serial.println(measureMsgPack(doc));

    const size_t size = measureMsgPack(doc);
    uint8_t* input = new uint8_t[size];
    serializeMsgPack(doc, input, size);

    DeserializationError error = deserializeMsgPack(doc, input);

    // Test if parsing succeeded.
    if (error) {
        Serial.print("deserializeMsgPack() failed: ");
        Serial.println(error.f_str());
        return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
    const char* sensor = doc["sensor"];
    long time = doc["time"];
    double latitude = doc["data"][0];
    double longitude = doc["data"][1];

    // Print values.
    Serial.println(sensor);
    Serial.println(time);
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
}

void loop() {
    // not used in this example
}
