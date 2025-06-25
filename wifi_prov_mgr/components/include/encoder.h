#ifndef _ENCODER_H
#define _ENCODER_H

#include "esp32_bsp.h"

extern volatile int32_t contador_encoder;
extern volatile int a_anterior;
extern volatile int b_anterior;

void encoder_init(void);

#endif // _ENCODER_H