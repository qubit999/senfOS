#ifndef BLINK_H
#define BLINK_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <iostream>

class PicoLED {
    public:
        PicoLED();
        void set(bool led_on);
        void blink(int times, int delay);
    private:
        void init();
};

#endif // BLINK_H
