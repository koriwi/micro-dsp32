#include "freertos/idf_additions.h"
#include "nvs_flash.h"
#include "webserver.h"
#include "wifi.h"
#include <stdio.h>
#include <string.h>

void webserver_task(void *pvParameters) {
  start_webserver();
  vTaskDelete(NULL);
}

void app_main(void) {
  nvs_flash_init();
  wifi_init_softap();

  // start webserver
  xTaskCreatePinnedToCore(webserver_task, "webserver task", 4096, NULL, 5, NULL,
                          0);
}
