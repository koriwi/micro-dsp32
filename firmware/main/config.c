#include "config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

config_t config[2];
/**
 * set all members of the config struct to the default values
 */
void init_config_with_defaults() {
  config[0].lp_freq = 430;
  config[0].lp_q = (uint16_t)32768 * 0.7f;
  config[0].lp_order = 0;

  config[0].hp_freq = 430;
  config[0].hp_q = (uint16_t)32768 * 0.7f;
  config[0].hp_order = 0;

  config[1].lp_freq = 430;
  config[1].lp_q = (uint16_t)32768 * 0.7f;
  config[1].lp_order = 0;

  config[1].hp_freq = 430;
  config[1].hp_q = (uint16_t)32768 * 0.7f;
  config[1].hp_order = 0;
}

/**
 * if there is already a config in the NVS, load it
 * if not, fill the config with defaults and write it to NVS
 */
esp_err_t load_or_init_nvs() {
  nvs_handle_t handle;
  esp_err_t err;
  err = nvs_open("eq", NVS_READONLY, &handle);

  if (err == ESP_OK) {
    ESP_LOGI("NVS", "NVS config found, loading...");
    size_t conf_size;
    nvs_get_blob(handle, "config", NULL, &conf_size);

    nvs_get_blob(handle, "config", &config, &conf_size);

    nvs_close(handle);
    return ESP_OK;
  }

  ESP_LOGI("NVS", "NVS empty, initializing...");

  err = nvs_open("eq", NVS_READWRITE, &handle);
  if (err != ESP_OK)
    return err;

  init_config_with_defaults();
  nvs_set_blob(handle, "config", &config, sizeof(config));
  nvs_commit(handle);

  nvs_close(handle);

  ESP_LOGI("NVS", "NVS now has config");
  return ESP_OK;
}

/**
 * save the current config struct to the NVS
 */
esp_err_t save_config() {
  nvs_handle_t handle;
  esp_err_t err;

  // check if config has ever been initialized
  err = nvs_open("eq", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }
  err = nvs_set_blob(handle, "config", &config, sizeof(config));
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_commit(handle);
  if (err != ESP_OK) {
    return err;
  }

  nvs_close(handle);
  return ESP_OK;
}

/**
 * save the device name for wifi ssid usage
 */

esp_err_t save_name(char *name) {
  nvs_handle_t handle;
  ESP_ERROR_CHECK(nvs_open("settings", NVS_READWRITE, &handle));

  ESP_ERROR_CHECK(nvs_set_str(handle, "name", name));

  ESP_ERROR_CHECK(nvs_commit(handle));
  nvs_close(handle);
  return ESP_OK;
}

/**
 * save the device name for wifi ssid usage
 */

esp_err_t get_name(char *name, size_t len) {
  nvs_handle_t handle;
  esp_err_t err;
  err = nvs_open("settings", NVS_READWRITE, &handle);

  size_t saved_len;
  char saved_name[20];
  err = nvs_get_str(handle, "name", NULL, &saved_len);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_ERROR_CHECK(nvs_set_str(handle, "name", ""));
    ESP_ERROR_CHECK(nvs_commit(handle));
    ESP_ERROR_CHECK(nvs_get_str(handle, "name", NULL, &saved_len));
  }
  ESP_ERROR_CHECK(nvs_get_str(handle, "name", saved_name, &saved_len));

  if (len < saved_len)
    saved_len = len;

  snprintf(name, saved_len, "%s", saved_name);

  nvs_close(handle);
  return ESP_OK;
}
