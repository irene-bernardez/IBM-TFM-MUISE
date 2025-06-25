idf_component_register(SRCS "src/esp32_bsp.c" "src/encoder.c" "src/motor.c" "src/wifi.c" "src/qr.c"
                      INCLUDE_DIRS "include" "${PROJECT_DIR}/main"
                      REQUIRES driver freertos esp_http_server esp_wifi nvs_flash wifi_provisioning qrcode)