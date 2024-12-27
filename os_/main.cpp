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
#include "libs/hotspot/hotspot.h"
#include "myhttpserver/myhttpserver.h"
#include "settings.h" // settings file

void setup() {
    try
    {
        stdio_init_all();
        sleep_ms(2000); // Give hardware time to stabilize
        if (cyw43_arch_init_with_country(CYW43_COUNTRY_GERMANY)) {
            printf("Failed to initialize WiFi\n");
            return;
        }
        
        Pico_OLED_1_3& oled2 = MyHTTPServer::getOLED(); 
        oled2.drawPage("senfOS (TM) 2024", ">BOOTING senfOS", ">>SETUP", ">>>LOAD...", 12);
        oled2.displayImage(epd_bitmap_senfos);
        if (SSID == "" || PASSWORD == "") {
            PicoLED led = PicoLED();
            oled2.drawPage("NO SSID/PASS SET", "STARTING HOTSPOT", "MODE", ">>>", 12);
            
            MyHotspot hotspot(HOTSPOT_NAME, HOTSPOT_PASS);
            
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
                cyw43_arch_poll();
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
        oled.drawPage(">>>CONNECTING", "WI-FI...", ">>>PLEASE WAIT", ">>>", 12);
        if (wifi.connect(SSID, PASSWORD) == false) {
            oled.drawPage("WI-FI CONNECTION FAILED", "", ">>>", ">>>RESTART DEVICE", 12);
            std::cerr << "WI-FI CONNECTION FAILED" << '\n';
            return -1;
        }
        oled.drawPage(">>>WI-FI:", ">>>CONNECTED", ">>>", ">>>", 12);
        oled.drawPage(">>>GET IP", ">>>", ">>>", ">>>", 12);
        const char* ip = wifi.showLocalIPv4();

        std::string ip_ext = http.http_get("api.ipify.org", 80, "") + std::to_string(HTTPD_SERVER_PORT);
        const char* service_ip = ip_ext.c_str();
        
        oled.drawPage("LOCAL IP: ", ip, "SERVICE IP: ", service_ip, 12);
        while (true) {
            cyw43_arch_poll();
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1; // Exit with error, Pico freezes
    }
    return 0;
}
