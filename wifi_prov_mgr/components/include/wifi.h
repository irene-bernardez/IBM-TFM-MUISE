#ifndef _WIFI_H
#define _WIFI_H

#include "esp_http_server.h"

void wifi_init(void);              // Nueva función
httpd_handle_t start_webserver(void);

#endif // _WIFI_H