#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

typedef union config_u {
  struct config_s {
    uint16_t lp_freq;
    uint16_t lp_q;
    uint16_t lp_order;
    uint16_t hp_freq;
    uint16_t hp_q;
    uint16_t hp_order;
  } obj;
  uint16_t arr[6];
} config_t;

config_t config;

void default_config() {
  config.obj.lp_freq = 430;
  config.obj.lp_q = (uint16_t)32768 * 0.7f;
  config.obj.lp_order = 0;

  config.obj.hp_freq = 430;
  config.obj.hp_q = (uint16_t)32768 * 0.7f;
  config.obj.hp_order = 0;
}

esp_err_t init_default_config(nvs_handle_t *handle) {
  esp_err_t err;

  err = nvs_open("eq", NVS_READWRITE, handle);
  if (err != ESP_OK)
    return err;

  default_config();
  nvs_set_blob(*handle, "config", &config, sizeof(config));
  nvs_commit(*handle);

  return ESP_OK;
}

esp_err_t get_config(char *buf, size_t buf_len) {
  nvs_handle_t handle;
  esp_err_t err;

  // check if config has ever been initialized
  err = nvs_open("eq", NVS_READONLY, &handle);
  if (err != ESP_OK) {
    // no, then do it
    err = init_default_config(&handle);
    if (err != ESP_OK)
      return err;
  }

  // passing null reads the size
  size_t conf_size;
  nvs_get_blob(handle, "config", NULL, &conf_size);

  nvs_get_blob(handle, "config", &config, &conf_size);
  snprintf(buf, buf_len, "lp_freq = %u, lp_q = %f", config.obj.lp_freq,
           config.obj.lp_q / 32768.0f);

  return ESP_OK;
}
