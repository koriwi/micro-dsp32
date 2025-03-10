#include "config.h"
#include "esp_err.h"
#include "esp_log.h"
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
  char buf[128];
  esp_err_t status = get_config(buf, 128);
  ESP_LOGW("NVS--------", "%i %s", status, buf);
  // start webserver
  xTaskCreatePinnedToCore(webserver_task, "webserver task", 4096, NULL, 5, NULL,
                          0);
}
