#ifndef __INFRARED_H
#define __INFRARED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DEV_Config.h"

extern int Infrared_PIN;

void SET_Infrared_PIN(uint8_t PIN);

int Read_Infrared_Value(void);

#ifdef __cplusplus
}
#endif

#endif
