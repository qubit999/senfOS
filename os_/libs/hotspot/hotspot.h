#ifndef MY_HOTSPOT_H
#define MY_HOTSPOT_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip4_addr.h"

class MyHotspot {
public:
    MyHotspot(const char *ssid, const char *pass);
    ~MyHotspot();
    bool start();
    void stop();
    bool is_active;
private:
    char current_ssid[33];
    char current_password[64];
};

#endif // MY_HOTSPOT_H
