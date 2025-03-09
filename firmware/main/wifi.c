#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include <string.h>

#define SSID "ESP32_AP"
#define PASSWORD "12345678"

void wifi_init_softap(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = {
      .ap = {.ssid = SSID,
             .password = PASSWORD,
             .ssid_len = strlen(SSID),
             .channel = 1,
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = 4},
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI("wifi", "Access Point \"%s\" started", SSID);
}
