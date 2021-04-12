#include <FastLED.h>

static constexpr uint8_t PIN_DATA {19};
static constexpr size_t N_LEDS {12};
CRGB leds[N_LEDS];

static constexpr uint8_t CORE_LED_SHOW {0};  // core 1: main app
static TaskHandle_t task_handle_led_show {0};
static TaskHandle_t task_handle_main_app {0};

void task_led_show(void* pvParameters) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // wait for the trigger
        FastLED.show();                           // do the show()
        xTaskNotifyGive(task_handle_main_app);    // notify back to the main app
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // prepare fastled
    FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, N_LEDS);

    // create the task
    xTaskCreatePinnedToCore(task_led_show, "task_led_show", 2048, NULL, 2, &task_handle_led_show, CORE_LED_SHOW);
}

void loop() {
    // change the led color
    update_led_data();

    // trigger FastLED.show() in 40fps
    static uint32_t prev_ms = millis();
    if (millis() >= prev_ms + 25) {
        prev_ms = millis();

        if (task_handle_main_app == 0) {                         // if FastLED.show() has done
            task_handle_main_app = xTaskGetCurrentTaskHandle();  // store the handle of this task
            xTaskNotifyGive(task_handle_led_show);               // trigger the show() task
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(200));        // wait until show() has done
            task_handle_main_app = 0;                            // clear task handle
        }
    }
}

void update_led_data() {
    float t = (float)millis() / 1000.f;
    float v = (0.5f * sin(t) + 0.5f) * 255.f;
    fill_solid(leds, N_LEDS, CRGB(v, v, v));
}
