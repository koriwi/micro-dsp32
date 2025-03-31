#include "cJSON.h"
#include "config.h"
#include "debug_audio.h"
#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "driver/i2s_types_legacy.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "hal/i2s_types.h"
#include "portmacro.h"
#include <math.h>
#include <stdint.h>

extern cJSON *config;
static i2s_chan_handle_t tx_chan; // I2S tx channel handler
static i2s_chan_handle_t rx_chan; // I2S rx channel handler

i2s_std_config_t i2s0_config = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_24BIT,
                                                    I2S_SLOT_MODE_STEREO),
    .gpio_cfg =
        {
            // .bclk = 25,
            // .ws = 22,
            // .dout = 32,
            // .din = 21,
            // .mclk = 0,

            // .bclk = 6,
            // .ws = 4,
            // .dout = 5,
            // .din = I2S_PIN_NO_CHANGE,
            // .mclk = 2,

            .bclk = 4,
            .ws = 32,
            .dout = 33,
            .din = 12,
            .mclk = 0,
        }

};

void i2s_init() {
  i2s0_config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
  i2s0_config.gpio_cfg.bclk =
      cJSON_GetObjectItemNested(config, "settings.i2s0.bclk")->valueint;
  i2s0_config.gpio_cfg.ws =
      cJSON_GetObjectItemNested(config, "settings.i2s0.ws")->valueint;
  i2s0_config.gpio_cfg.dout =
      cJSON_GetObjectItemNested(config, "settings.i2s0.dout")->valueint;
  i2s0_config.gpio_cfg.din =
      cJSON_GetObjectItemNested(config, "settings.i2s0.din")->valueint;
  i2s0_config.gpio_cfg.mclk =
      cJSON_GetObjectItemNested(config, "settings.i2s0.mclk")->valueint;
  int i2s0_master =
      cJSON_GetObjectItemNested(config, "settings.i2s0.master")->valueint;

  i2s_chan_config_t chan0_cfg =
      I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, (i2s_role_t)i2s0_master);
  i2s_new_channel(&chan0_cfg, &tx_chan, &rx_chan);

  ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &i2s0_config));
  ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &i2s0_config));
}

// i2s helper
size_t bytes_written = 0;
size_t bytes_read = 0;

int32_t i2s_buffer[128];

void start_i2s_audio() {

  ESP_LOGI("I2S", "I2S Task started on Core %d", xPortGetCoreID());
  i2s_channel_enable(rx_chan);
  i2s_channel_enable(tx_chan);
  while (1) {
    i2s_channel_read(rx_chan, i2s_buffer, sizeof(i2s_buffer), &bytes_read,
                     portMAX_DELAY);
    // generate_sine_wave(i2s_rx_buffer,
    //                    sizeof(i2s_rx_buffer) / sizeof(i2s_rx_buffer[0]));
    // for (int i = 0; i < sizeof(i2s_buffer) / sizeof(i2s_buffer[0]); i++) {
    //   i2s_buffer[i] = (int32_t)(i2s_buffer[i] >> 8);
    // }
    i2s_channel_write(tx_chan, i2s_buffer, bytes_read, &bytes_written,
                      portMAX_DELAY);
    // vTaskDelay(1);
  }
}
