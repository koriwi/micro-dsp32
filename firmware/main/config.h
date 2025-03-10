#include "esp_err.h"

typedef struct {
  uint16_t lp_freq;
  uint16_t lp_q;
  uint16_t lp_order;
  uint16_t hp_freq;
  uint16_t hp_q;
  uint16_t hp_order;
} config_t;
esp_err_t load_or_init_nvs();
esp_err_t save_config();
