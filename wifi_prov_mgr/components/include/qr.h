#ifndef _QR_H
#define _QR_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"

void wifi_prov_init(void);    // Función para iniciar la provisión
void wifi_prov_deinit(void);  // Opcional: para limpiar recursos

#endif // _QR_H