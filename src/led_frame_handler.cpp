#include <led_frame_handler.h>

/*
============================================================================
                               RMT management                               
============================================================================
*/

static rmt_data_t *lfh_rmt_data;
static size_t lfh_rmt_data_size;
static rmt_obj_t *lfh_rmt_handle = NULL;

void lfh_rmt_alloc()
{
  // Calculate needed size to hold a full frame
  lfh_rmt_data_size = (
    vars_get_num_pixels() // Number of frames
    * 3 // Three pixels per led
    * 8 // Eight bits per pixel
  );

  // Allocate that many structs
  lfh_rmt_data = (rmt_data_t *) malloc(sizeof(rmt_data_t) * lfh_rmt_data_size);
  if (!lfh_rmt_data)
  {
    dbg_log("Could not allocate rmt_data_t pixel-bit buffer!\n");
    return;
  }

  dbg_log("Allocated %" PRIu32 " rmt_data_t's for the RMT buffer!\n", lfh_rmt_data_size);

  // Initialize important fields
  for (int i = 0; i < lfh_rmt_data_size; i++)
  {
    rmt_data_t *curr = &lfh_rmt_data[i];
    curr->level0 = 1;
    curr->level1 = 0;
    curr->duration0 = 4;
    curr->duration1 = 8;
  }

  // Allocate RMT "device" if not existing yet
  if (lfh_rmt_handle == NULL)
  {
    lfh_rmt_handle = rmtInit(LFH_LED_DATA_PIN, true, RMT_MEM_64);
    if (!lfh_rmt_handle)
    {
      dbg_log("Could not allocate rmt handle!");
      return;
    }

    // Set the clock divider
    rmtSetTick(lfh_rmt_handle, 100);
    dbg_log("Allocated RMT \"device\" and set the clock divider!\n");
  }
}

void lfh_rmt_dealloc()
{
  // Free buffer
  free(lfh_rmt_data);
  lfh_rmt_data_size = 0;
}

void lfh_rmt_copy_frame(uint8_t *frame_data, uint32_t frame_size)
{
  // Iterate frames
  for (int i = 0; i < frame_size; i += 3)
  {
    // Extract individual pixels
    uint32_t color_com = (
      frame_data[i] << 16) | // R byte
      (frame_data[i + 1] << 8) | // G byte
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
      rmt_data_t *curr_dat = &lfh_rmt_data[
        (i / 3) // Pixel index
        * (8 * 3) // Offset per pixel
        + j // Pixel's bit index
      ];

      // Apply durations
      curr_dat->duration0 = pix_sta ? LFH_LED_DUR_T10_US : LFH_LED_DUR_T00_US;
      curr_dat->duration1 = pix_sta ? LFH_LED_DUR_T11_US : LFH_LED_DUR_T01_US;
    }
  }
}

void lfh_rmt_write_frame()
{
  if (!rmtWrite(lfh_rmt_handle, lfh_rmt_data, lfh_rmt_data_size))
  {
    dbg_log("Could not write data using rmt!\n");
    return;
  }
}

/*
============================================================================
                               Basic control                                
============================================================================
*/

static File lfh_framebuf;
static long lfh_last_render;
static uint32_t lfh_current_frame;
static uint8_t *frame_buf = NULL; //[frame_size] = { 0 };

Adafruit_NeoPixel strip(180, LFH_LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

void lfh_init()
{
  // Open frame onto local frame handle
  sdh_open_frames_file("r", &lfh_framebuf);

  // Reset frame timing
  lfh_last_render = millis();
  lfh_current_frame = 0;

  // Allocate RMT related resources
  // lfh_rmt_alloc();

  frame_buf = (uint8_t *) malloc(lfh_get_frame_size());

  strip.begin();
  strip.show();
}

void lfh_deinit()
{
  if (lfh_framebuf) lfh_framebuf.close();
  // lfh_rmt_dealloc();

  if (frame_buf)
    free(frame_buf);
}

/*
============================================================================
                             Framebuffer write                              
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
                             Framebuffer read                               
============================================================================
*/

bool lfh_read_frame(uint16_t frame_index, uint8_t *out_buf)
{
  // Persistent buffer not available
  if (!lfh_framebuf) return false;

  uint16_t frame_size = lfh_get_frame_size();

  // Could not seek frame start
  if (!lfh_framebuf.seek(frame_size * frame_index)) return false;

  // Not enough bytes remaining
  if (lfh_framebuf.available() < frame_size) return false;

  // Read frame into external buffer
  lfh_framebuf.read(out_buf, frame_size);
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

/**
 * @brief Draw a frame to the pixels
 * 
 * @param frame_data Frame data buffer containing pixel color data
 * @param frame_size Size of one frame in bytes
 */
void lfh_draw_frame2(uint8_t *frame_data, uint32_t frame_size)
{
  // Each pixel has to have three color components
  if (frame_size % 3 != 0)
  {
    dbg_log("Invalid frame_size of %" PRIu32 " provided!\n", frame_size);
    return;
  }

  // for (int i = 0; i < frame_size; i += 3)
  //   dbg_log("(%" PRIu8 ", %" PRIu8 ", %" PRIu8 ")\n", frame_data[i], frame_data[i + 1], frame_data[i + 2]);

  // Copy frame data into RMT bit-buffer and write it out
  lfh_rmt_copy_frame(frame_data, frame_size);
  lfh_rmt_write_frame();

  dbg_log("Drawed %" PRIu32 "!\n", lfh_current_frame);
}

void lfh_draw_frame(uint8_t *frame_data, uint32_t frame_size)
{
  for (int i = 0; i < frame_size; i+=3)
  {
    uint8_t r = frame_data[i], g = frame_data[i + 1], b = frame_data[i + 2];
    strip.setPixelColor(i / 3, r, g, b);
  }
  strip.show();
}

void lfh_handle_frame()
{
  // Inter-frame delay timer
  // INFO: Through this, the frame will take *at least* frame_dur, but may
  // INFO: take longer, if the rendering itself takes longer
  if (millis() - lfh_last_render < vars_get_frame_dur()) return;
  lfh_last_render = millis();

  // Check if file is open, otherwise processing is probably paused
  if (!lfh_framebuf) return;

  uint32_t frame_size = vars_get_num_pixels() * 3;
  long start = millis();

  // File not big enough, thus no frame info available from here on onwards, reset
  if (!lfh_framebuf.seek(frame_size * lfh_current_frame))
  {
    dbg_log("No next LED frame found, resetting!\n");
    lfh_current_frame = 0;
    return;
  }

  // File has not enough bytes remaining, thus no frame info available from here on onwards, reset
  if (lfh_framebuf.available() < frame_size)
  {
    dbg_log("Not enough LED frame data remaining, resetting!\n");
    lfh_last_render = millis() - vars_get_frame_dur();
    lfh_current_frame = 0;
    return;
  }

  // Read frame into buffer and draw
  lfh_framebuf.read(frame_buf, frame_size);
  long stop = millis();
  dbg_log("File read took around %ldms!\n", stop - start);

  start = millis();
  lfh_draw_frame(frame_buf, frame_size);
  stop = millis();
  dbg_log("Pixel draw took around %ldms!\n", stop - start);

  // Advance to next frame
  if (++lfh_current_frame == vars_get_num_frames())
    lfh_current_frame = 0;
}