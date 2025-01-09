#ifndef EPD_213_H
#define EPD_213_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()
#include "EPD_2in13_V4.h"

#ifdef __cplusplus
}
#endif

class epd_213 { 
public:
    epd_213();
    ~epd_213();
    void drawPage(const char* text1, const char* text2, const char* text3, const char* text4, int fontSize);
    void displayImage(const UBYTE* image);

private:
    UBYTE* BlackImage;
    void init();
};

#endif // EPD_213_H