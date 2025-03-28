#include "config.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
cJSON *config;
// Function to get a nested JSON object using a dot-separated path (supports
// arrays)
cJSON *cJSON_GetObjectItemNested(cJSON *json, const char *path) {
  if (!json || !path)
    return NULL;

  char path_copy[256]; // Adjust size as needed
  strncpy(path_copy, path, sizeof(path_copy));
  path_copy[sizeof(path_copy) - 1] = '\0'; // Ensure null termination

  char *token = strtok(path_copy, ".");
  cJSON *current = json;

  while (token && current) {
    // Check if token is an array index (e.g., "data.0.value")
    char *bracket = strchr(token, '[');
    if (bracket) {
      *bracket = '\0'; // Split key and index
      cJSON *array = cJSON_GetObjectItem(current, token);
      if (!array || !cJSON_IsArray(array))
        return NULL; // Ensure it's an array

      int index = atoi(bracket + 1); // Convert index string to integer
      current = cJSON_GetArrayItem(array, index);
    } else {
      current = cJSON_GetObjectItem(current, token);
    }

    token = strtok(NULL, ".");
  }

  return current;
}

/**
 * set all members of the config struct to the default values
 */
void init_config_with_defaults() {
  uint8_t mac[6];
  esp_wifi_get_mac(WIFI_IF_AP, mac);

  char name[20];
  snprintf(name, sizeof(name), "%02X%02X%02X", mac[3], mac[4], mac[5]);

  config = cJSON_CreateObject();
  cJSON *settings = cJSON_AddObjectToObject(config, "settings");

  cJSON *wifi = cJSON_AddObjectToObject(settings, "wifi");
  cJSON_AddStringToObject(wifi, "name", name);

  cJSON *i2s0 = cJSON_AddObjectToObject(settings, "i2s0");
  cJSON_AddNumberToObject(i2s0, "ws", 22);
  cJSON_AddNumberToObject(i2s0, "bck", 32);
  cJSON_AddNumberToObject(i2s0, "mck", 27);
  cJSON_AddNumberToObject(i2s0, "sdo", 25);
  cJSON_AddNumberToObject(i2s0, "sdi", 21);
  cJSON *i2s1 = cJSON_AddObjectToObject(settings, "i2s1");
  cJSON_AddNumberToObject(i2s1, "ws", 14);
  cJSON_AddNumberToObject(i2s1, "bck", 15);
  cJSON_AddNumberToObject(i2s1, "mck", 16);
  cJSON_AddNumberToObject(i2s1, "sdo", 16);
  cJSON_AddNumberToObject(i2s1, "sdi", 16);

  cJSON *eq = cJSON_AddArrayToObject(config, "eq");

  for (int i = 0; i < 2; i++) {
    cJSON *channel = cJSON_CreateObject();
    cJSON *lp = cJSON_AddObjectToObject(channel, "lowpass");
    cJSON *hp = cJSON_AddObjectToObject(channel, "highpass");

    cJSON_AddNumberToObject(lp, "freq", 430);
    cJSON_AddNumberToObject(lp, "order", 0);
    cJSON_AddNumberToObject(lp, "q", 0.7);
    cJSON_AddNumberToObject(hp, "freq", 430);
    cJSON_AddNumberToObject(hp, "order", 0);
    cJSON_AddNumberToObject(hp, "q", 0.7);
    cJSON_AddItemToArray(eq, channel);
  }
}

/**
 * if there is already a config in the NVS, load it
 * if not, fill the config with defaults and write it to NVS
 */
esp_err_t load_or_init_nvs() {
  nvs_handle_t handle;
  esp_err_t err;
  err = nvs_open("config", NVS_READONLY, &handle);

  if (err == ESP_OK) {
    ESP_LOGI("NVS", "NVS config found, loading...");
    size_t conf_size;
    nvs_get_str(handle, "config", NULL, &conf_size);
    char *conf_str = malloc(conf_size);
    nvs_get_str(handle, "config", conf_str, &conf_size);

    config = cJSON_Parse(conf_str);

    nvs_close(handle);
    free(conf_str);
    return ESP_OK;
  }

  ESP_LOGI("NVS", "NVS empty, initializing...");

  err = nvs_open("config", NVS_READWRITE, &handle);
  if (err != ESP_OK)
    return err;

  init_config_with_defaults();
  char *conf_str = cJSON_PrintUnformatted(config);
  nvs_set_str(handle, "config", conf_str);
  nvs_commit(handle);

  nvs_close(handle);
  free(conf_str);

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
  err = nvs_open("config", NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }
  char *config_str = cJSON_PrintUnformatted(config);
  err = nvs_set_str(handle, "config", config_str);
  free(config_str);
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
