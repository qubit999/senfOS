#ifndef OLED_H
#define OLED_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <string>

#include "EPD_Test.h"
#include "LCD_1in14.h"
#include "OLED_1in3_c.h"
#include "ImageData.h"
#include "GUI_Paint.h"
#include "image.h"

// Implementation of 1.3" OLED Display Module for Pico, embedded SH1107 driver, using SPI/I2C bus

class Pico_OLED_1_3 {
public:
    Pico_OLED_1_3();
    ~Pico_OLED_1_3();
    void drawPage(const char* text1, const char* text2, const char* text3, const char* text4, int fontSize);
    void displayImage(const UBYTE* image);
    void handleInput(const char* text1, const char* text2, const char* text2_1, const char* text2_2, const char* boot1, const char* boot2, int fontSize);

private:
    UBYTE* BlackImage;
    void init();
};

#endif // OLED_H