#include <led_frame_handler.h>

/*
============================================================================
                               RMT management                               
============================================================================
*/

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

/*
============================================================================
                               Basic control                                
============================================================================
*/

static File lfh_frame_file;
static long lfh_last_render;
static uint32_t lfh_current_frame;
static uint16_t lfh_frame_num_blocks;
static uint8_t *lfh_frame_ringbuf;
static lfh_paused_t lfh_pause_req = 0;
static void *lfh_pause_req_arg = 0;
static bool lfh_is_paused = false;

/**
 * @brief Initialize the frame ringbuffer with off pixels
 */
void lfh_frame_ringbuf_init()
{
  for (int pixel = 0; pixel < lfh_frame_num_blocks * LFH_FRAME_RINGBUF_BS * LFH_FRAME_RINGBUF_SLOTS; pixel++)
    lfh_frame_ringbuf[pixel] = 0;

  dbg_log("Initialized the frame ringbuffer!\n");
}

/**
 * @brief Allocate the frame ringbuffer based on the
 * frame number of blocks
 */
void lfh_frame_ringbuf_alloc()
{
  size_t total_size = 
    lfh_frame_num_blocks // Total number of blocks required
    * LFH_FRAME_RINGBUF_SLOTS // This many times
    * LFH_FRAME_RINGBUF_BS; // With this size per block

  lfh_frame_ringbuf = (uint8_t *) malloc(total_size);
  dbg_log("Allocated %" PRIu32 " bytes's for the frame ringbuffer!\n", total_size);
}

/**
 * @brief Free the frame ringbuffer's memory
 */
void lfh_frame_ringbuf_dealloc()
{
  free (lfh_frame_ringbuf);
}

void lfh_init()
{
  // Open frame onto local frame handle
  sdh_open_frames_file("r", &lfh_frame_file);

  // Reset frame timing
  lfh_last_render = millis();
  lfh_current_frame = 0;

  // Calculate the number of blocks a frame takes up (padded to fit)
  lfh_frame_num_blocks = ceil((double) (vars_get_num_pixels() * 3.0) / (double) LFH_FRAME_RINGBUF_BS);

  // Allocate and initialize frame ringbuffer
  lfh_frame_ringbuf_alloc();
  lfh_frame_ringbuf_init();

  // Read first frame so that there's always one more frame read than used
  lfh_read_frame();

  // Initialize RMT
  lfh_rmt_init();
}

void lfh_deinit()
{
  // Close frame file
  if (lfh_frame_file) lfh_frame_file.close();

  // Dellocate frame ringbuffer
  lfh_frame_ringbuf_dealloc();

  // End RMT
  lfh_rmt_dealloc();
}

void lfh_pause(lfh_paused_t done, void *arg)
{
  // Already paused, instantly call
  if (lfh_is_paused)
  {
    done(arg);
    return;
  }

  // Mark for request
  // WARNING: If two requests are tight enough that they both have
  // WARNING: to wait for pause, one will override the other.
  lfh_pause_req = done;
  lfh_pause_req_arg = arg;
}

void lfh_resume()
{
  lfh_pause_req = 0;
  lfh_is_paused = false;

  // Open frame onto local frame handle
  sdh_open_frames_file("r", &lfh_frame_file);
  dbg_log("Resumed frame processing!\n");
}

/*
============================================================================
                             Frame file write                               
============================================================================
*/

bool lfh_init_file()
{
  if (!sdh_delete_frames_file())
  {
    dbg_log("Could not delete the old frames file!\n");
    return false;
  }

  File handle;

  // Open file for write access
  sdh_open_frames_file("w", &handle);

  // Could not open file
  if (!handle)
  {
    dbg_log("Could not open frames file!\n");
    return false;
  }

  // Write empty frames to it
  uint8_t empty_pixel[3] = { 0 };
  for (uint16_t i = 0; i < vars_get_num_frames(); i++)
  {
    // Write empty frame
    for (uint16_t j = 0; j < vars_get_num_pixels(); j++)
      handle.write(empty_pixel, sizeof(empty_pixel));
  }

  dbg_log(
    "Initialized new frames file for num_pixels=%" PRIu16 " and num_frames=%" PRIu16 " !\n",
    vars_get_num_pixels(),
    vars_get_num_frames()
  );

  // Close resource
  handle.close();
  return true;
}

bool lfh_write_frame(uint16_t frame_index, uint8_t *frame_data)
{
  File handle;

  sdh_open_frames_file("r+", &handle);

  // Could not open file
  if (!handle) return false;

  // Go to frame start
  if (!handle.seek(lfh_get_frame_size() * frame_index))
  {
    dbg_log("Frame file wasn't large enough!\n");

    // Re-try after trying to create a new file
    if (lfh_init_file())
      return lfh_write_frame(frame_index, frame_data);

    return false;
  }

  // Override currently selected frame with that data
  size_t frame_data_pointer = 0;
  uint8_t pixel_data_buf[3] = {};

  for (uint16_t i = 0; i < vars_get_num_pixels(); i++)
  {
    memcpy(pixel_data_buf, &frame_data[frame_data_pointer], 3);
    handle.write(pixel_data_buf, 3);
    frame_data_pointer += 3;
  }

  handle.close();

  dbg_log(
    "Overwrote frame %" PRIu16 " at offset %" PRIu32 "!\n",
    frame_index,
    lfh_get_frame_size() * frame_index
  );

  return true;
}

/*
============================================================================
                             Frame file read                                
============================================================================
*/

bool lfh_read_frame()
{

  static size_t framebuf_slot = 0, frame_ind = 0;

  // Persistent buffer not available
  if (!lfh_frame_file) return false;

  // Go to start of current frame's block
  if (!lfh_frame_file.seek(lfh_get_frame_size() * frame_ind))
  {
    dbg_log("LED frame %" PRIu32 " not available, resetting index!\n", frame_ind);
    frame_ind = 0;
    return lfh_read_frame();
  }

  // Read info buffer block by block
  for (int i = 0; i < lfh_frame_num_blocks; i++)
  {
    // INFO: This will "overshoot" at the last frame, reading less bytes, as the
    // INFO: byte-size is not always perfectly divisible by 512
    lfh_frame_file.readBytes(
      (char *) &lfh_frame_ringbuf[
        framebuf_slot * LFH_FRAME_RINGBUF_BS * lfh_frame_num_blocks // Offset for current frame
        + i * LFH_FRAME_RINGBUF_BS // Offset for current block of frame
      ],
      LFH_FRAME_RINGBUF_BS // One block
    );
  }

  // Advance to next slot, wrapping around
  if (++framebuf_slot == LFH_FRAME_RINGBUF_SLOTS)
    framebuf_slot = 0;

  // Advance to next frame
  if (++frame_ind == vars_get_num_frames())
  {
    frame_ind = 0;
    lfh_frame_file.seek(0);
  }

  // Process pause request
  if (lfh_pause_req)
  {
    // Close frame file
    if (lfh_frame_file) lfh_frame_file.close();
    dbg_log("Paused frame processing!\n");

    // Set local flag
    lfh_is_paused = true;

    // Invoke callback
    lfh_pause_req(lfh_pause_req_arg);
  }

  return true;
}

bool lfh_read_frame_content(uint16_t frame_index, uint8_t *data_buf)
{
  // Could not open file
  if (!sdh_open_frames_file("r", &lfh_frame_file)) return false;

  // Start of frame out of range
  if (!lfh_frame_file.seek(lfh_get_frame_size() * frame_index)) return false;

  // Not enough data for a full frame
  if (lfh_frame_file.available() < lfh_get_frame_size()) return false;

  // Read info buffer
  lfh_frame_file.readBytes((char *) data_buf, lfh_get_frame_size());

  // Close file, success
  lfh_frame_file.close();
  return true;
}

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

uint16_t lfh_get_frame_slots()
{
  return LFH_MAX_FRAMES;
}

uint16_t lfh_get_frame_size()
{
  return vars_get_num_pixels() * 3;
}

uint16_t lfh_get_max_num_pixels()
{
  return LFH_MAX_PIXELS;
}

/*
============================================================================
                              Frame handling                                
============================================================================
*/

#define LHF_CONST_FRAME_TIME 10

void lfh_handle_frame()
{
  static size_t ringbuf_slot = 0;

  // Read next frame in advance
  // TODO: Invoke on second core for max. speed
  if (!lfh_read_frame()) return;

  // Copy frame from ringbuffer into RMT data buffer by transforming it to bits
  lfh_rmt_copy_frame(&lfh_frame_ringbuf[
    ringbuf_slot * lfh_frame_num_blocks * LFH_FRAME_RINGBUF_BS
  ], lfh_get_frame_size());

  // Write out RMT data
  rmt_write_items(RMT_CHANNEL_0, (rmt_item32_t *) lfh_rmt_tx_data, lfh_rmt_tx_data_size, true);

  // Inter-frame delay
  static long last_render = millis();
  while (millis() - last_render <= LHF_CONST_FRAME_TIME);
  last_render = millis();

  // Advance ringbuffer slot index, wrapping around
  if (++ringbuf_slot == LFH_FRAME_RINGBUF_SLOTS)
    ringbuf_slot = 0;
}