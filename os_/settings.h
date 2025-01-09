#define SSID ""
#define PASSWORD ""
#define APN "internet.t-d1.de" // APN for German T-Mobile SIM card, will be implemented later
#define HOTSPOT_NAME "senf-hotspot"
#define HOTSPOT_PASS "mittelscharf"
#define DISPLAY_MODULE 1 // 1 = Pico-OLED-1.3, 2 = Pico-ePaper-2.13 (v4)
// HTTP SERVER PORT can be changed in ./myhttpserver/lwipopts.h => line 57
// The HTTP Server serves the same files for HOTSPOT MODE and WI-FI MODE