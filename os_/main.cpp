#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <iostream>
#include <string>
#include <exception>
#include <pico/cyw43_arch.h>
#include <pico/lwip_nosys.h>
#include "lwip/apps/httpd.h"
#include "oled.h"
#include "libs/blink/blink.h"
#include "libs/myhttp/myhttp.h"
#include "libs/mywifi/mywifi.h"
#include "myhttpserver/myhttpserver.h"
#include "libs/hotspot/hotspot.h"
#include "settings.h" // settings file

int counter = 0;

void refreshIPs(Pico_OLED_1_3& oled, MyWiFi& wifi) {
    counter++;
    if(counter % 60 == 0) {
        MyHTTP myhttp;
        oled.drawPage(">>>REFRESHING", ">>>FETCHING IPS", ">>>", ">>>", 12);
        sleep_ms(2000);
        const char* ip_refresh = wifi.showLocalIPv4();
        std::string ext_ip_refresh = myhttp.http_get("api.ipify.org", 80, "") + ":" + std::to_string(HTTPD_SERVER_PORT);
        const char* service_ip_refresh = ext_ip_refresh.c_str();
        printf("Service IP is: %s\n", service_ip_refresh);
        sleep_ms(2000);
        oled.drawPage("LOCAL IP: ", ip_refresh, "SERVICE IP: ", service_ip_refresh, 12);
        counter = 0;
    }
}

void setup() {
    try
    {
        stdio_init_all();
        sleep_ms(2000); // Give hardware time to stabilize
        cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY);
        Pico_OLED_1_3& oled2 = MyHTTPServer::getOLED(); 
        oled2.drawPage("senfOS (TM) 2024", ">BOOTING senfOS", ">>SETUP", ">>>LOAD...", 12);
        sleep_ms(2000);
        oled2.displayImage(epd_bitmap_senfos);
        sleep_ms(8000);
        if (SSID == "" || PASSWORD == "") {
            PicoLED led = PicoLED();
            oled2.drawPage("NO SSID/PASS SET", "STARTING HOTSPOT", "MODE", ">>>", 12);
            sleep_ms(3000);
            
            MyHotspot hotspot(HOTSPOT_NAME, HOTSPOT_PASS);
            sleep_ms(1000); 
            
            if (!hotspot.start()) {
                oled2.drawPage("HOTSPOT ERROR", "FAILED TO START", ">>>", ">>>", 12);
                return;
            }
            
            MyHTTPServer hotspot_server;
            std::string ssid = std::string("") + std::string(HOTSPOT_NAME);
            std::string pass = std::string("") + std::string(HOTSPOT_PASS);
            std::string ip_adr = std::string("192.168.4.1:") + std::string(std::to_string(HTTPD_SERVER_PORT));
            oled2.drawPage("HOTSPOT ACTIVE", ssid.c_str(), pass.c_str(), ip_adr.c_str(), 12);
            
            while(true) {
                led.blink(1,100);
                sleep_ms(100);
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
        Pico_OLED_1_3& oled = MyHTTPServer::getOLED(); 
        oled.drawPage(">>>HTTP SERVER", ">>>STARTED", ">>>", ">>>", 12);
        sleep_ms(3000);
        oled.drawPage(">>>CONNECTING", "WI-FI...", ">>>PLEASE WAIT", ">>>", 12);
        led.blink(3, 500);
        if (wifi.connect(SSID, PASSWORD) == false) {
            led.blink(5, 500);
            oled.drawPage("WI-FI CONNECTION FAILED", "", ">>>", ">>>RESTART DEVICE", 12);
            std::cerr << "WI-FI CONNECTION FAILED" << '\n';
            return -1;
        }
        while(wifi.showLocalIPv4() == "No IP assigned") {
            sleep_ms(1000);
        }
        while(wifi.showLocalIPv6() == "No IPv6 assigned") {
            sleep_ms(1000);
        }
        oled.drawPage("WI-FI:", ">>>CONNECTED", ">>>FETCHING IPS", ">>>", 12);
        sleep_ms(2000);
        const char* ip = wifi.showLocalIPv4();
        std::string ext_ip = http.http_get("api.ipify.org", 80, "") + ":" + std::to_string(HTTPD_SERVER_PORT);
        const char* service_ip = ext_ip.c_str();
        oled.drawPage("LOCAL IP: ", ip, "SERVICE IP: ", service_ip, 12);
        while (true) {
            refreshIPs(oled, wifi);
            led.blink(1, 100);
            cyw43_arch_poll();
            sleep_ms(100);
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1; // Exit with error, Pico freezes
    }
    return 0;
}