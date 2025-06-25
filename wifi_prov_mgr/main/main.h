#ifndef _MAIN_H
#define _MAIN_H

#include "freertos/FreeRTOS.h"  // Añadir esto
#include "freertos/semphr.h"

extern SemaphoreHandle_t sem_enc_init;
extern SemaphoreHandle_t sem_motor_init;
extern volatile bool init_stepper;
extern volatile bool startup_done;

#endif // _MAIN_H