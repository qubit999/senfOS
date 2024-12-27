#ifndef OLED_CPP
#define OLED_CPP
#include "oled.h"

// Constructor
Pico_OLED_1_3::Pico_OLED_1_3() {
    init();
}

// Destructor
Pico_OLED_1_3::~Pico_OLED_1_3() {
    free(BlackImage);
    DEV_Module_Exit();
}

// Initialization method
void Pico_OLED_1_3::init() {
    DEV_Delay_ms(100);
    printf("OLED_test Demo\r\n");
    if (DEV_Module_Init() != 0) {
        while (1) {
            printf("END\r\n");
        }
    }
    OLED_1in3_C_Init();
    OLED_1in3_C_Clear();
    UWORD Imagesize = ((OLED_1in3_C_WIDTH % 8 == 0) ? (OLED_1in3_C_WIDTH / 8) : (OLED_1in3_C_WIDTH / 8 + 1)) * OLED_1in3_C_HEIGHT;
    if ((BlackImage = (UBYTE*)malloc(Imagesize)) == NULL) {
        while (1) {
            printf("Failed to apply for black memory...\r\n");
        }
    }
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);
}

// Method implementations
void Pico_OLED_1_3::drawPage(const char* text1, const char* text2, const char* text3, const char* text4, int fontSize) {
    Paint_SelectImage(BlackImage);
    Paint_Clear(BLACK);
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
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(100);
    Paint_Clear(BLACK);
}

void Pico_OLED_1_3::displayImage(const UBYTE* image) {
    OLED_1in3_C_Display(image);
    DEV_Delay_ms(100);
}

void Pico_OLED_1_3::handleInput(const char* text1, const char* text2, const char* text2_1, const char* text2_2, const char* boot1, const char* boot2, int fontSize) {
    Paint_SelectImage(BlackImage);
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
    int key0 = 15;
    int key1 = 17;
    int key = 0;
    DEV_GPIO_Mode(key0, 0);
    DEV_GPIO_Mode(key1, 0);
    while (DEV_Digital_Read(key0) != 0 && DEV_Digital_Read(key1) != 0) {
        Paint_DrawString_EN(10, 0, boot1, font, WHITE, BLACK);
        Paint_DrawString_EN(10, 34, boot2, font, WHITE, BLACK);
        OLED_1in3_C_Display(BlackImage);
        DEV_Delay_ms(100);
    }
    Paint_Clear(BLACK);
    OLED_1in3_C_Display(BlackImage);
    while (true) {
        // Clear the image buffer
        Paint_Clear(BLACK);
        if (DEV_Digital_Read(key0) == 0) {
            Paint_DrawString_EN(10, 0, text1, font, WHITE, BLACK);
            Paint_DrawString_EN(10, 34, text2, font, WHITE, BLACK);
            key = 1;
        } else if (DEV_Digital_Read(key1) == 0) {
            Paint_DrawString_EN(10, 0, text2_1, font, WHITE, BLACK);
            Paint_DrawString_EN(10, 34, text2_2, font, WHITE, BLACK);
            key = 2;
        }
        if (key == 1) {
            Paint_DrawString_EN(10, 0, text1, font, WHITE, BLACK);
            Paint_DrawString_EN(10, 34, text2, font, WHITE, BLACK);
        } else if (key == 2) {
            Paint_DrawString_EN(10, 0, text2_1, font, WHITE, BLACK);
            Paint_DrawString_EN(10, 34, text2_2, font, WHITE, BLACK);
        }
        OLED_1in3_C_Display(BlackImage);
        DEV_Delay_ms(100);
    }
}

#endif // OLED_CPP