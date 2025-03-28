#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/i2s_types.h"
#include "portmacro.h"
#include <math.h>
#include <stdint.h>

static i2s_chan_handle_t tx_chan; // I2S tx channel handler
static i2s_chan_handle_t rx_chan; // I2S rx channel handler

i2s_std_config_t i2s_config = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
    .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT,
                                                    I2S_SLOT_MODE_STEREO),
    .gpio_cfg =
        {
            .bclk = 25,
            .ws = 22,
            .dout = 32,
            .din = 21,
            .mclk = 0,

        }

};

void i2s_init() {
  i2s_config.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
  // i2s_config.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
  i2s_chan_config_t chan_cfg =
      I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_cfg, &tx_chan, &rx_chan);

  ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &i2s_config));
  ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &i2s_config));
}

// i2s helper
size_t bytes_written = 0;
size_t bytes_read = 0;

int32_t i2s_rx_buffer[128];

void start_i2s_audio() {

  ESP_LOGI("I2S", "I2S Task started on Core %d", xPortGetCoreID());
  i2s_channel_enable(rx_chan);
  i2s_channel_enable(tx_chan);
  while (1) {
    i2s_channel_read(rx_chan, i2s_rx_buffer, sizeof(i2s_rx_buffer), &bytes_read,
                     portMAX_DELAY);

    i2s_channel_write(tx_chan, i2s_rx_buffer, bytes_read, &bytes_written,
                      portMAX_DELAY);
  }
}
