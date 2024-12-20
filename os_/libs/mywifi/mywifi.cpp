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
    if (cyw43_arch_wifi_connect_async(
            ssid, password, CYW43_AUTH_WPA2_AES_PSK) == 0) {
        printf("Wi-Fi connected\n");
        return true;
    } else {
        printf("Failed to connect to Wi-Fi\n");
        return false;
    }
    return false;
}

const char* MyWiFi::showLocalIPv4() {
    // Get local IPv4 address
    struct netif *netif = netif_list;
    static char ip_str[16];
    while (netif != NULL) {
        if (netif_is_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            ip4addr_ntoa_r(netif_ip4_addr(netif), ip_str, sizeof(ip_str));
            return ip_str;
        }
        netif = netif->next;
    }
    return "No IP assigned";
}

const char* MyWiFi::showLocalIPv6() {
    // Get local IPv6 address
    struct netif *netif = netif_list;
    static char ip6_str[IP6ADDR_STRLEN_MAX];
    
    while (netif != NULL) {
        if (netif_is_up(netif)) {
            for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
                    ip6_addr_islinklocal(netif_ip6_addr(netif, i))) {
                    ip6addr_ntoa_r(netif_ip6_addr(netif, i), ip6_str, sizeof(ip6_str));
                    return ip6_str;
                }
            }
        }
        netif = netif->next;
    }
    return "No IPv6 assigned";
}
