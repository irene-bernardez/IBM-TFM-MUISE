#ifndef _MOTOR_H
#define _MOTOR_H

#include "esp32_bsp.h"
#include "driver/gptimer.h"

extern volatile int32_t desired_steps;  // Añadir esta línea
extern volatile bool motor_active;
extern volatile int32_t motor_position;
extern gptimer_handle_t timer_handle;

void motor_init(void);

#endif // _MOTOR_H