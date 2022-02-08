#include <lfh/lfh_rmt.h>

// RMT transmission buffer
static rmt_data_t *lfh_rmt_tx_data;
static size_t lfh_rmt_tx_data_size;

/**
 * @brief Initialize the RMT transmission buffer with standard values
 */
void lfh_rmt_tx_data_init()
{
  // Initialize important fields
  for (int i = 0; i < lfh_rmt_tx_data_size; i++)
  {
    rmt_data_t *curr = &lfh_rmt_tx_data[i];

    // Level 0 is always HIGH while Level 1 will always be LOW
    curr->level0 = 1;
    curr->level1 = 0;

    // Dummy values, get patched per frame individually
    curr->duration0 = 1;
    curr->duration1 = 1;
  }

  dbg_log("Initialized the RMT transmission buffer!\n");
}

/**
 * @brief Allocate the RMT transmission buffer
 */
void lfh_rmt_tx_data_alloc()
{
  // Calculate needed size to hold a full frame
  lfh_rmt_tx_data_size = (
    vars_get_num_pixels() // Number of frames
    * 3 // Three pixels per led
    * 8 // Eight bits per pixel
  );

  // Allocate that many data-structs
  lfh_rmt_tx_data = (rmt_data_t *) malloc(sizeof(rmt_data_t) * lfh_rmt_tx_data_size);
  if (!lfh_rmt_tx_data)
  {
    dbg_log("Could not allocate rmt_data_t pixel-bit buffer!\n");
    return;
  }

  dbg_log("Allocated %" PRIu32 " rmt_data_t's for the RMT transmission buffer!\n", lfh_rmt_tx_data_size);
}

/**
 * @brief Free the RMT transmission buffer
 */
void lfh_rmt_dealloc()
{
  // Free buffer
  free(lfh_rmt_tx_data);
  lfh_rmt_tx_data_size = 0;
}

/**
 * @brief Initialize the RMT devic
 */
void lfh_rmt_init()
{
  // Create transmission buffer
  lfh_rmt_tx_data_alloc();
  lfh_rmt_tx_data_init();

  // Configure device
  rmt_config_t cfg = {
    RMT_MODE_TX, RMT_CHANNEL_0, // Transmit on CH0
    LFH_LED_CLK_DIV,
    (gpio_num_t) LFH_LED_DATA_PIN,
    0x1,
    {
      false, // No loop
      0, 0, RMT_CARRIER_LEVEL_LOW, false, // No carrier
      RMT_IDLE_LEVEL_LOW, true // Low on idle
    }
  };

  // Install configuration
  rmt_config(&cfg);
  rmt_driver_install(cfg.channel, 0, 0);
}

void lfh_rmt_copy_frame(uint8_t *frame_data, uint32_t frame_size)
{
  // Iterate frames
  for (int i = 0; i < frame_size; i += 3)
  {
    // Extract individual pixels
    uint32_t color_com = (
      frame_data[i + 1] << 16) | // G byte
      (frame_data[i] << 8) | // R byte
      (frame_data[i + 2] // B byte
    );

    // Iterate bits within this pixel
    for (int j = 0; j < 8 * 3; j++)
    {
      // TODO: Think about R G B byte position macros here, as it's swapped now
      // TODO: and depends on the chip built into the LED

      // Mask out individual bits of this pixel, reverse bit-order
      bool pix_sta = color_com & (1 << (8 * 3 - j - 1));

      // Get a pointer to the current data
      rmt_data_t *curr_dat = &lfh_rmt_tx_data[
        (i / 3) // Pixel index
        * (8 * 3) // Offset per pixel
        + j // Pixel's bit index
      ];

      // Apply durations
      curr_dat->duration0 = pix_sta ? LFH_LED_DUR_T1H_US : LFH_LED_DUR_T0H_US;
      curr_dat->duration1 = pix_sta ? LFH_LED_DUR_T1L_US : LFH_LED_DUR_T0L_US;
    }
  }
}

void lfh_rmt_write_items(bool async)
{
  rmt_write_items(RMT_CHANNEL_0, (rmt_item32_t *) lfh_rmt_tx_data, lfh_rmt_tx_data_size, !async);
}