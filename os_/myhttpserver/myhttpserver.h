#ifndef MY_HTTPSERVER_H
#define MY_HTTPSERVER_H

#include <string>
#include "oled.h"

class MyHTTPServer {
public:
    int port;
    MyHTTPServer();
    ~MyHTTPServer();
    void start();
    void start_cgi();
    static void debugInfo(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
    static Pico_OLED_1_3& getOLED() { return oled; }
    static Pico_OLED_1_3 oled;
};

#endif