#include "mywifi.h"

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
#define IP6ADDR_STRLEN_MAX 46

MyWiFi::MyWiFi() {
    cyw43_arch_enable_sta_mode();
}

MyWiFi::~MyWiFi() {
    cyw43_arch_deinit();
}

bool MyWiFi::connect(const char* ssid, const char* password) {
    // Connect to Wi-Fi
    printf("Connecting to Wi-Fi...\n");
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (cyw43_arch_wifi_connect_async(ssid, password, CYW43_AUTH_WPA2_AES_PSK) != 0) {
        if (to_ms_since_boot(get_absolute_time()) - start > 30000) {
            printf("Failed to connect to Wi-Fi\n");
            return false;
        }
        cyw43_arch_poll();
    }
    printf("Wi-Fi connected\n");
    
    // Wait for DHCP to assign an IP address
    start = to_ms_since_boot(get_absolute_time());
    while (netif_ip4_addr(netif_default)->addr == 0) {
        if (to_ms_since_boot(get_absolute_time()) - start > 30000) {
            printf("Failed to obtain IP address via DHCP\n");
            return false;
        }
        cyw43_arch_poll();
    }
    printf("IP address obtained\n");
    return true;
}

const char* MyWiFi::showLocalIPv4() {
    if (netif_default == NULL) {
        return "No IP";
    }
    static char ip_str[16];
    cyw43_arch_lwip_begin();
    ip4_addr_t ip = *netif_ip4_addr(netif_default);
    cyw43_arch_lwip_end();
    if (!ip4_addr_isany_val(ip)) {
        ip4addr_ntoa_r(&ip, ip_str, sizeof(ip_str));
        return ip_str;
    }
    return "No IP";
}

const char* MyWiFi::showLocalIPv6() {
    if (netif_default == NULL) {
        return "No IP";
    }
    static char ip6_str[IP6ADDR_STRLEN_MAX];
    if (netif_default != NULL) {
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_isvalid(netif_ip6_addr_state(netif_default, i))) {
                cyw43_arch_lwip_begin();
                const ip6_addr_t *addr = netif_ip6_addr(netif_default, i);
                cyw43_arch_lwip_end();
                if (!ip6_addr_isany(addr)) {
                    ip6addr_ntoa_r(addr, ip6_str, sizeof(ip6_str));
                    return ip6_str;
                }
            }
        }
    }
    return "No IP";
}
