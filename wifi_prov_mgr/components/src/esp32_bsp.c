#include "esp32_bsp.h"
#include "driver/gpio.h"

void bsp_gpio_init(void) {
    gpio_config_t motor_out = {
        .pin_bit_mask = (1ULL<<GPIO_STEP)|(1ULL<<GPIO_DIR)|(1ULL<<GPIO_ENBL)
                      |(1ULL<<GPIO_MODE0)|(1ULL<<GPIO_MODE1)|(1ULL<<GPIO_MODE2)
                      |(1ULL<<GPIO_RST)|(1ULL<<GPIO_SLP),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&motor_out);

    gpio_config_t enc = {
        .pin_bit_mask = (1ULL<<ENCODER_A)|(1ULL<<ENCODER_B),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE
    };
    gpio_config(&enc);

    gpio_config_t lim = {
        .pin_bit_mask = (1ULL<<LIMIT_MOTOR_POS)|(1ULL<<LIMIT_MOTOR_NEG)|(1ULL<<LIMIT_ENCODER_NEG),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .intr_type    = GPIO_INTR_NEGEDGE
    };
    gpio_config(&lim);
}