#include "audio.h"
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
void audio_task(void *pvParameters) {
  start_i2s_audio();
  vTaskDelete(NULL);
}

void app_main(void) {
  nvs_flash_init();
  load_or_init_nvs();

  wifi_init_softap();
  i2s_init();
  // start webserver
  xTaskCreatePinnedToCore(webserver_task, "webserver task", 4096, NULL, 5, NULL,
                          0);
  xTaskCreatePinnedToCore(audio_task, "audio task", 4096, NULL, 5, NULL, 1);
}
