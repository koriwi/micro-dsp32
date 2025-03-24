#include "cJSON.h"
#include "esp_err.h"

// cJSON *config;
esp_err_t load_or_init_nvs();
esp_err_t save_config();
cJSON *cJSON_GetObjectItemNested(cJSON *json, const char *path);
