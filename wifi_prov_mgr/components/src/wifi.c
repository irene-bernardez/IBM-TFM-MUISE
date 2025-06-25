#include "wifi.h"
#include "main.h"
#include "encoder.h"
#include "motor.h"
#include "esp_http_server.h"
#include "freertos/event_groups.h"
#include "qr.h"  // Nueva inclusión
#include "esp_log.h"  // Añadir esta línea

static const char *WS_TAG = "WS_SERVER";

static esp_err_t ws_handler(httpd_req_t *req) {
    ESP_LOGI(WS_TAG, "Cliente WebSocket conectado");
    char data[128];
    TickType_t tick = xTaskGetTickCount();
    while (1) {
        if (!startup_done) {
            if (!init_stepper) {
                snprintf(data, sizeof(data), "{\"message\": \"Espere, preparándose. Mueva el brazo sano a la posición inicial.\"}");
            } else {
                snprintf(data, sizeof(data), "{\"message\": \"Ajustando brazo afectado a la posición inicial...\"}");
            }
        } else {
            snprintf(data, sizeof(data), "{\"encoder\": %" PRId32 ", \"motor\": %" PRId32 ", \"message\": \"Sistema listo, terapia iniciada.\"}", contador_encoder, motor_position);
        }
        httpd_ws_frame_t ws_pkt = {
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t *)data,
            .len = strlen(data)
        };
        if (httpd_ws_send_frame(req, &ws_pkt) != ESP_OK) break;
        xTaskDelayUntil(&tick, pdMS_TO_TICKS(20));
    }
    ESP_LOGI(WS_TAG, "Cliente WebSocket desconectado");
    return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    FILE *file = fopen("/spiffs/index.html", "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, file);
    buf[len] = 0;
    fclose(file);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, buf, len);
    free(buf);
    return ESP_OK;
}

httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t ws_uri = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = ws_handler,
        .is_websocket = true
    };
    httpd_uri_t root_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_get_handler
    };

    httpd_register_uri_handler(server, &ws_uri);
    httpd_register_uri_handler(server, &root_uri);
    return server;
}

void wifi_init(void) {
    wifi_prov_init();  // Llamada a la función de provisión de QR
}