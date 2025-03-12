#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SSID_BASE "micro-dsp32-"
#define PASSWORD "microdsp32"

void wifi_init_softap(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  uint8_t mac[6];
  esp_wifi_get_mac(WIFI_IF_AP, mac);

  char ssid[32];
  snprintf(ssid, sizeof(ssid), "%s%02X%02X%02X", SSID_BASE, mac[3], mac[4],
           mac[5]);

  wifi_config_t wifi_config = {
      .ap = {.ssid = "",
             .password = PASSWORD,
             .ssid_len = 0,
             .channel = 1,
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = 4},
  };

  strcpy((char *)wifi_config.ap.ssid, ssid);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI("wifi", "Access Point \"%s\" started", ssid);
}
