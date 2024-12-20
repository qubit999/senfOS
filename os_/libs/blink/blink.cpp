#include "blink.h"

PicoLED::PicoLED() {
    #ifndef LED_DELAY_MS
    #define LED_DELAY_MS 100
    #endif
    init();
}

void PicoLED::set(bool led_on) {
    #if defined(PICO_DEFAULT_LED_PIN)
            gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
}

void PicoLED::blink(int times, int delay) {
    delay = delay / 2;
    for (int i = 0; i < times; i++) {
        set(true);
        sleep_ms(delay);
        set(false);
        sleep_ms(delay);
    }
}

void PicoLED::init() {
    #if defined(PICO_DEFAULT_LED_PIN)
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    #elif defined(CYW43_WL_GPIO_LED_PIN)      
    #endif
}
