#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <exception>
#include <pico/cyw43_arch.h>
#include <pico/lwip_nosys.h>
#include "lwip/apps/httpd.h"
#include "oled.h"
// #include "epd_213.h"
// #include "image.h"
#include "libs/blink/blink.h"
#include "libs/myhttp/myhttp.h"
#include "libs/mywifi/mywifi.h"
#include "libs/hotspot/hotspot.h"
#include "myhttpserver/myhttpserver.h"
#include "settings.h" // settings file

int counter = 0;

void refreshIPs(auto& display, MyWiFi& wifi) {
    counter++;
    if(counter % 600 == 0) {
        MyHTTP myhttp;
        display.drawPage(">>>REFRESH", ">>>GET IPs", ">>>", ">>>", 12);
        const char* ip_refresh = wifi.showLocalIPv4();
        std::string ext_ip_refresh = myhttp.http_get("api.ipify.org", 80, "");
        const char* service_ip_refresh = ext_ip_refresh.c_str();
        printf("Service IP is: %s\n", service_ip_refresh);
        display.drawPage("LOCAL IP: ", ip_refresh, "SERVICE IP: ", service_ip_refresh, 12);
        counter = 0;
    }
}

void setup() {
    try
    {
        stdio_init_all();
        sleep_ms(2000); // Give hardware time to stabilize
        if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY)) {
            printf("Failed to initialize WiFi\n");
            return;
        }
        #if (DISPLAY_MODULE == 1)
                Pico_OLED_1_3 &display = MyHTTPServer::getOLED();
        #else
                epd_213 &display = MyHTTPServer::getEPD();
        #endif
        display.drawPage("senfOS (TM) 2024", ">BOOTING senfOS", ">>SETUP", ">>>LOAD...", 12);
        sleep_ms(2000);
        #if (DISPLAY_MODULE == 1)
            display.displayImage(epd_bitmap_senfos);
        #else
            display.displayImage(epd_bitmap_senfos2);
        #endif
        sleep_ms(5000);
        if (SSID == "" || PASSWORD == "") {
            PicoLED led = PicoLED();
            display.drawPage("NO SSID/PASS SET", "STARTING HOTSPOT", "MODE", ">>>", 12);
            
            MyHotspot hotspot(HOTSPOT_NAME, HOTSPOT_PASS);
            
            if (!hotspot.start()) {
                display.drawPage("HOTSPOT ERROR", "FAILED TO START", ">>>", ">>>", 12);
                return;
            }
            
            MyHTTPServer hotspot_server;
            std::string ssid = std::string("") + std::string(HOTSPOT_NAME);
            std::string pass = std::string("") + std::string(HOTSPOT_PASS);
            std::string ip_adr = std::string("192.168.4.1");
            display.drawPage("HOTSPOT ACTIVE", ssid.c_str(), pass.c_str(), ip_adr.c_str(), 12);
            
            while(true) {
                cyw43_arch_poll();
                led.blink(1, 100);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main() {
    setup();
    try {
        PicoLED led = PicoLED();
        MyWiFi wifi = MyWiFi();
        MyHTTP http = MyHTTP();
        MyHTTPServer server = MyHTTPServer();
        #if (DISPLAY_MODULE == 1)
                Pico_OLED_1_3 &display = MyHTTPServer::getOLED();
        #else
                epd_213 &display = MyHTTPServer::getEPD();
        #endif
        display.drawPage(">>>HTTP SERVER", ">>>STARTED", ">>>", ">>>", 12);
        display.drawPage(">>>CONNECTING", "WI-FI...", ">>>PLEASE WAIT", ">>>", 12);
        wifi.connect(SSID, PASSWORD);
        while(wifi.showLocalIPv4() == "No IP") {
            sleep_ms(1000);
        }
        while(wifi.showLocalIPv6() == "No IP") {
            sleep_ms(1000);
        }
        display.drawPage(">>>WI-FI:", ">>>CONNECTED", ">>>", ">>>", 12);
        sleep_ms(1000);
        display.drawPage(">>>GET IPs", ">>>", ">>>", ">>>", 12);
        sleep_ms(2000);
        const char* ip = wifi.showLocalIPv4();
        std::string ip_ext = http.http_get("api.ipify.org", 80, "");
        const char* service_ip = ip_ext.c_str();
        display.drawPage("LOCAL IP:", ip, "SERVICE IP:", service_ip, 12);
        while (true) {
            cyw43_arch_poll();
            led.blink(1, 100);
            refreshIPs(display, wifi);
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1; // Exit with error, Pico freezes
    }
    return 0;
}
