#ifndef _ESP32_BSP_H
#define _ESP32_BSP_H

#include "driver/gpio.h"  // Necesario para GPIO_NUM_X

// Declaración de la función
void bsp_gpio_init(void);

// =============================================================================
// 1) DEFINICIÓN DE PINES (GPIO)
// -----------------------------------------------------------------------------

// A) Pines DRV8825
#define GPIO_STEP           GPIO_NUM_6   // Cada conmutacion genera un paso del motor
#define GPIO_DIR            GPIO_NUM_5   // Indica la direccion de rotacion del motor
#define GPIO_ENBL           GPIO_NUM_2    // Activo a nivel bajo. 0 = motor habilitado; 1 = motor deshabilitado 

#define GPIO_RST            GPIO_NUM_0 
#define GPIO_SLP            GPIO_NUM_7 

// Pines para el microstepping del DRV8825. El micorstepping hace que el motor se mueva mas suave y reduce el ruido y el calentamiento
#define GPIO_MODE0          GPIO_NUM_11    
#define GPIO_MODE1          GPIO_NUM_10
#define GPIO_MODE2          GPIO_NUM_1

// B) Pines encoder
#define ENCODER_A           GPIO_NUM_15   // Encoder canal A
#define ENCODER_B           GPIO_NUM_23   // Encoder canal B

// C) Pines interruptores limites
#define LIMIT_MOTOR_POS     GPIO_NUM_20
#define LIMIT_MOTOR_NEG     GPIO_NUM_19
#define LIMIT_ENCODER_NEG   GPIO_NUM_21

// D) Parámetros                  
#define STEP_HZ               18000                    // Frec. base de STEP (Hz). Determina la velocidad. El driver DRV8825 puede soportar como maximo 250kHz
#define STEP_PERIOD_US        (1000000 / STEP_HZ)      // Microsegundos (µs) entre conmutacion de STEP
#define IDLE_TIMEOUT_MS       50                       // ms sin pulsos antes de sleep. Tiempo de inactividad 50 ms (sin pulsos) antes de desactivar el motor
#define MICROSTEPS            8                        // Microstepping del DRV8825. 1, 2, 4, 8 o 16 microsteps por paso completo. Ver datasheet para saber como confirgurar el microstepping

// *** Distancia del encoder: ***
// Circunferencia de la polea = 16 dientes x 2 mm/diente = 32 mm
// Pulsos por vuelta = 800 x 2 = 1600 (si cuentas flancos de subida y bajada debes multiplicarlo por 2)
// dencoder = 32 / 1600 = 0.02 mm/pulso

// *** Distancia del motor: ***
// Paso por tornillo = 8 mm/vuelta
// Pasos por vuelta = 200 x 8 = 1600 (siendo 8 el microstepping configurado)
// dmotor = 8 / 1600 = 0.005 mm/pulso

// *** Relación entre encoder y motor: ***
// dmotor * 0.005 = dencoder * 0.02 -->  dmotor = (0.02/0.005) * dencoder = 4 mm
#define MICROSTEPS_PER_PULSE  4   

#endif //_ESP32_BSP_H