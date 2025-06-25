#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_spiffs.h"             // SPIFFS: sistema de archivos en flash
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.h"
#include "esp32_bsp.h"
#include "encoder.h"
#include "motor.h"
#include "main.h"
#include "esp_spiffs.h"             // SPIFFS: sistema de archivos en flash

static const char *TAG = "main";

SemaphoreHandle_t sem_enc_init = NULL;
SemaphoreHandle_t sem_motor_init = NULL;
volatile bool init_stepper = false;
volatile bool startup_done = false;

// =============================================================================
// INICIALIZACIÓN DE SPIFFS: sistema de archivos en flash para recursos web
// -----------------------------------------------------------------------------
static void init_spiffs(void) {
    ESP_LOGI("SPIFFS", "Inicializando SPIFFS...");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",         // Punto de montaje en VFS
        .partition_label = NULL,
        .max_files = 5,                 // Límite de archivos abiertos simultáneos
        .format_if_mount_failed = true  // Formatea flash si no está formateado
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Error mounting SPIFFS: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPIFFS", "SPIFFS montado correctamente");
    }
}


static void startup_routine(void) {
    ESP_LOGI(TAG, "[Startup] Mueva el encoder a posición 0 (tope negativo encoder)");
    xSemaphoreTake(sem_enc_init, portMAX_DELAY);
    ESP_LOGI(TAG, "[Startup] Encoder en posición inicial confirmado");

    ESP_LOGI(TAG, "[Startup] Moviendo motor hacia tope negativo");
    gpio_set_level(GPIO_ENBL, 0);
    motor_active = true;
    init_stepper = true;
    desired_steps = 1000000;
    xSemaphoreTake(sem_motor_init, portMAX_DELAY);
    ESP_LOGI(TAG, "[Startup] Motor en posición inicial confirmado");

    desired_steps = 0;
    contador_encoder = 0;
    motor_position = 0;
    startup_done = true;
    ESP_LOGI(TAG, "[Startup] Terapia iniciada");
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    init_spiffs();
    wifi_init();  // Cambia esto
    start_webserver();

    sem_enc_init = xSemaphoreCreateBinary();
    sem_motor_init = xSemaphoreCreateBinary();

    bsp_gpio_init();
    encoder_init();
    motor_init();

    startup_routine();

    ESP_LOGI(TAG, "Sync: %d Hz, %d us/step, %d µsteps/pulso, sleep tras %d ms",
             STEP_HZ, STEP_PERIOD_US, MICROSTEPS_PER_PULSE, IDLE_TIMEOUT_MS);
}