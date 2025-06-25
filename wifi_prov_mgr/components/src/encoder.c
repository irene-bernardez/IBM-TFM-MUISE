#include "encoder.h"
#include "motor.h"  // Añadir esta inclusión
#include "main.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "encoder";

volatile int32_t contador_encoder = 0;
volatile int a_anterior = 0;
volatile int b_anterior = 0;

static void IRAM_ATTR encoder_isr_handler(void *arg) {
    if (!startup_done) return;

    int a = gpio_get_level(ENCODER_A);
    int b = gpio_get_level(ENCODER_B);
    int dir = 0;

    if (a==0 && a_anterior==1 && b==1 && b_anterior==1) dir = -1;
    else if (a==0 && a_anterior==0 && b==0 && b_anterior==1) dir = -1;
    else if (a==1 && a_anterior==0 && b==0 && b_anterior==0) dir = -1;
    else if (a==1 && a_anterior==1 && b==1 && b_anterior==0) dir = -1;
    else if (a==1 && a_anterior==1 && b==0 && b_anterior==1) dir = 1;
    else if (a==0 && a_anterior==1 && b==0 && b_anterior==0) dir = 1;
    else if (a==0 && a_anterior==0 && b==1 && b_anterior==0) dir = 1;
    else if (a==1 && a_anterior==0 && b==1 && b_anterior==1) dir = 1;

    contador_encoder += dir;
    desired_steps += dir * MICROSTEPS_PER_PULSE;
    a_anterior = a;
    b_anterior = b;
}

static void IRAM_ATTR encoder_limit_isr(void *arg) {
    if (gpio_get_level(LIMIT_ENCODER_NEG) == 0) {
        BaseType_t higher = pdFALSE;
        xSemaphoreGiveFromISR(sem_enc_init, &higher);
        if (higher) portYIELD_FROM_ISR();
        ESP_EARLY_LOGI(TAG, "[Startup] Encoder en posición inicial detectado");
    }
}

void encoder_init(void) {
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENCODER_A, encoder_isr_handler, NULL);
    gpio_isr_handler_add(ENCODER_B, encoder_isr_handler, NULL);
    gpio_isr_handler_add(LIMIT_ENCODER_NEG, encoder_limit_isr, NULL);
}