#ifndef EPD_213_CPP
#define EPD_213_CPP

#include "epd_213.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()
#include "EPD_2in13_V4.h"

epd_213::epd_213() {
    init();
}

epd_213::~epd_213() {
	EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();

    EPD_2in13_V4_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);
    DEV_Module_Exit();
}

void epd_213::init() {
    if (DEV_Module_Init() != 0) {
        Debug("Module Init Failed\r\n");
        return;
    }

    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0) ? (EPD_2in13_V4_WIDTH / 8) : (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return;
    }

    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);
    Paint_Clear(WHITE);
}

void epd_213::drawPage(const char* text1, const char* text2, const char* text3, const char* text4, int fontSize) {
    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);
    EPD_2in13_V4_Init();
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    sFONT* font;
    switch (fontSize) {
        case 8:
            font = &Font8;
            break;
        case 12:
            font = &Font12;
            break;
        case 16:
            font = &Font16;
            break;
        default:
            font = &Font8;
            break;
    }
    Paint_DrawString_EN(10, 0, text1, font, WHITE, BLACK);
    Paint_DrawString_EN(10, 17, text2, font, WHITE, BLACK);
    Paint_DrawString_EN(10, 34, text3, font, WHITE, BLACK);
    Paint_DrawString_EN(10, 51, text4, font, WHITE, BLACK);
    EPD_2in13_V4_Display_Base(BlackImage);
    DEV_Delay_ms(3000);
}

void epd_213::displayImage(const UBYTE* image) {
    EPD_2in13_V4_Init_Fast();
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(image);
    EPD_2in13_V4_Display_Fast(BlackImage);
    DEV_Delay_ms(2000);
}

#endif // EPD_213_CPP