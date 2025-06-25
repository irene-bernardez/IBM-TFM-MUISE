#include "freertos/FreeRTOS.h"  // Añadir esto al inicio
#include "motor.h"
#include "main.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"

static const char *TAG = "motor";

volatile int32_t desired_steps = 0;
volatile bool motor_active = false;
volatile int32_t motor_position = 0;
gptimer_handle_t timer_handle = NULL;
static volatile bool step_state = false;
static volatile uint8_t motor_microstep_counter = 0;
static int idle_callback_count = 0;
static const int idle_threshold = (STEP_HZ * IDLE_TIMEOUT_MS) / 1000;

// Mover la definición de timer_alarm_cb antes de init_gptimer
bool IRAM_ATTR timer_alarm_cb(gptimer_handle_t handle, const gptimer_alarm_event_data_t *edata, void *user_data) {
    if (desired_steps == 0) {
        idle_callback_count++;
        if (motor_active && idle_callback_count >= idle_threshold) {
            gpio_set_level(GPIO_ENBL, 1);
            motor_active = false;
        }
        if (step_state) {
            step_state = false;
            gpio_set_level(GPIO_STEP, 0);
        }
        return false;
    }

    idle_callback_count = 0;
    if (!motor_active) {
        gpio_set_level(GPIO_ENBL, 0);
        motor_active = true;
    }

    bool forward = (desired_steps > 0);
    gpio_set_level(GPIO_DIR, forward);

    step_state = !step_state;
    gpio_set_level(GPIO_STEP, step_state);

    if (!step_state) {
        if (forward) {
            desired_steps -= 1;
            motor_microstep_counter++;
            if (motor_microstep_counter >= MICROSTEPS_PER_PULSE) {
                motor_position += 1;
                motor_microstep_counter = 0;
            }
        } else {
            desired_steps += 1;
            motor_microstep_counter++;
            if (motor_microstep_counter >= MICROSTEPS_PER_PULSE) {
                motor_position -= 1;
                motor_microstep_counter = 0;
            }
        }
    }
    return false;
}

static void init_gptimer(void) {
    gptimer_config_t cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&cfg, &timer_handle));

    gptimer_alarm_config_t alarm = {
        .alarm_count = STEP_PERIOD_US,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm));

    gptimer_event_callbacks_t cbs = { .on_alarm = timer_alarm_cb };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(timer_handle));
    ESP_ERROR_CHECK(gptimer_start(timer_handle));
}

static void IRAM_ATTR limit_motor_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t)(uintptr_t)arg;
    if (init_stepper && gpio_num == LIMIT_MOTOR_NEG) {
        BaseType_t higher = pdFALSE;
        xSemaphoreGiveFromISR(sem_motor_init, &higher);
        init_stepper = false;
        if (higher) portYIELD_FROM_ISR();
        ESP_EARLY_LOGI(TAG, "[Startup] Motor en posición inicial detectado");
        return;
    }
    if (startup_done && (gpio_num == LIMIT_MOTOR_NEG || gpio_num == LIMIT_MOTOR_POS)) {
        desired_steps = 0;
        ESP_EARLY_LOGI(TAG, "Límite MOTOR alcanzado: pasos pendientes reseteados");
    }
}

void motor_init(void) {
    gpio_set_level(GPIO_ENBL, 1); 
    gpio_set_level(GPIO_DIR, 1);
    gpio_set_level(GPIO_MODE0, 1);
    gpio_set_level(GPIO_MODE1, 1);
    gpio_set_level(GPIO_MODE2, 0);
    gpio_set_level(GPIO_RST, 1);
    gpio_set_level(GPIO_SLP, 1);

    gpio_isr_handler_add(LIMIT_MOTOR_POS, limit_motor_isr_handler, (void*)(uintptr_t)LIMIT_MOTOR_POS);
    gpio_isr_handler_add(LIMIT_MOTOR_NEG, limit_motor_isr_handler, (void*)(uintptr_t)LIMIT_MOTOR_NEG);

    init_gptimer();
}