#include "hotspot.h"
#include <pico/cyw43_arch.h>
#include <pico/lwip_nosys.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include "pico/stdlib.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip6.h"

MyHotspot::MyHotspot(const char* ssid, const char* pass) : is_active(false) {
    strncpy(current_ssid, ssid, sizeof(current_ssid) - 1);
    strncpy(current_password, pass, sizeof(current_password) - 1);
    current_ssid[sizeof(current_ssid) - 1] = '\0';
    current_password[sizeof(current_password) - 1] = '\0';
}

MyHotspot::~MyHotspot() {
    stop();
}

bool MyHotspot::start() {
    if (strlen(current_ssid) > 32 || strlen(current_password) < 4) {
        printf("Invalid SSID or password length\n");
        return false;
    }

    cyw43_arch_enable_ap_mode(current_ssid, current_password, CYW43_AUTH_WPA3_WPA2_AES_PSK);

    is_active = true;
    printf("AP started successfully\n");
    return true;
}

void MyHotspot::stop() {
    if (is_active) {
        cyw43_arch_disable_ap_mode();
        cyw43_arch_deinit();
        is_active = false;
    }
}