#ifndef MYWIFI_H
#define MYWIFI_H

#include <stdio.h>
#include <string.h>
#include <cstring>
#include "pico/stdlib.h"

class MyWiFi {
public:
    MyWiFi();
    ~MyWiFi();
    bool connect(const char* ssid, const char* password);
    const char* showLocalIPv4();
    const char* showLocalIPv6();
};

#endif // MYWIFI_H
