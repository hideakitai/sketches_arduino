#include <FastLED.h>

// FastLED
static constexpr uint8_t PIN_DATA {32};
static constexpr size_t N_LEDS {16};
CRGBArray<N_LEDS> leds;

// FreeRTOS
static TaskHandle_t handle_main_app {NULL};
static TaskHandle_t handle_task_led_show {NULL};
static constexpr uint8_t CORE_TASK_LED_SHOW {0};  // core 1: main app
static constexpr uint32_t STACK_SIZE {2048};      // bytes
static constexpr uint32_t PRIORITY {2};           // LOW 0 - 25 HIGH

void isr_task_led_show(void* pvParameters) {
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // wait for the trigger
        FastLED.show();                           // do the show()
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(handle_main_app, &xHigherPriorityTaskWoken);  // notify back to the main app
    }
}

void trigger_led_show() {
    const uint32_t r = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1));  // check for notification
    if (r == pdTRUE) {                                              // if isr_task_led_show has done
        xTaskNotifyGive(handle_task_led_show);                      // trigger the next show() task
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // prepare fastled
    FastLED.addLeds<NEOPIXEL, PIN_DATA>(leds, N_LEDS);

    // store the handle of this task
    handle_main_app = xTaskGetCurrentTaskHandle();

    // create the isr task for fastled show()
    xTaskCreatePinnedToCore(
        isr_task_led_show,
        "isr_task_led_show",
        STACK_SIZE,
        NULL,
        PRIORITY,
        &handle_task_led_show,
        CORE_TASK_LED_SHOW);

    xTaskNotifyGive(handle_task_led_show);  // trigger the first show() task
}

void loop() {
    // change the led color
    leds.fill_rainbow(uint8_t(float(millis() % 3000) / 3000.f * 255.f));

    // trigger FastLED.show() in 125fps
    static uint32_t prev_ms = millis();
    if (millis() >= prev_ms + 8) {
        prev_ms = millis();
        trigger_led_show();
    }
}
