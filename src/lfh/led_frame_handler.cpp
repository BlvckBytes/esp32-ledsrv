#include <lfh/led_frame_handler.h>

/*
============================================================================
                               Basic control                                
============================================================================
*/

static long lfh_last_render;
static uint8_t *lfh_frame_ringbuf;

/**
 * @brief Allocate the frame ringbuffer based on the
 * frame number of blocks
 */
void lfh_frame_ringbuf_alloc()
{
  size_t total_size = 
    LFH_FRAME_RINGBUF_SLOTS // This many times
    * vars_get_num_pixels() * 3; // This many frames

  lfh_frame_ringbuf = (uint8_t *) malloc(total_size);
  dbg_log("Allocated %" PRIu32 " bytes's for the frame ringbuffer!\n", total_size);

  // Initialize
  for (size_t i = 0; i < total_size; i++)
    lfh_frame_ringbuf[i] = 0;

  dbg_log("Initialized the frame ringbuffer!\n");
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
  lfh_frame_file_open();

  // Reset frame timing
  lfh_last_render = millis();

  // Allocate and initialize frame ringbuffer
  lfh_frame_ringbuf_alloc();

  // Read first frame so that there's always one more frame read than used
  lfh_read_frame();

  // Initialize RMT
  lfh_rmt_init();
}

void lfh_deinit()
{
  // Close frame file
  lfh_frame_file_close();

  // Dellocate frame ringbuffer
  lfh_frame_ringbuf_dealloc();

  // End RMT
  lfh_rmt_dealloc();
}

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

uint16_t lfh_get_max_num_pixels()
{
  return LFH_MAX_PIXELS;
}

/*
============================================================================
                              Frame handling                                
============================================================================
*/

bool lfh_read_frame()
{
  static size_t ringbuf_slot = 0;
  static size_t frame_index = 0;

  if (!lfh_frame_file_read(
    frame_index,
    &lfh_frame_ringbuf[ringbuf_slot * vars_get_num_pixels() * 3]
  )) return false;

  // Advance to next frame
  if (++frame_index == vars_get_num_frames())
    frame_index = 0;

  // Advance ringbuffer slot index, wrapping around
  if (++ringbuf_slot == LFH_FRAME_RINGBUF_SLOTS)
    ringbuf_slot = 0;

  return true;
}

#define LHF_CONST_FRAME_TIME 10

void lfh_handle_frame()
{
  static size_t ringbuf_slot = 0;

  // Read next frame in advance
  // TODO: Invoke on second core for max. speed
  if (!lfh_read_frame()) return;

  // Copy frame from ringbuffer into RMT data buffer by transforming it to bits
  lfh_rmt_copy_frame(&lfh_frame_ringbuf[
    ringbuf_slot * vars_get_num_pixels() * 3
  ], vars_get_num_pixels() * 3);

  // Write out RMT data
  lfh_rmt_write_items();

  // Inter-frame delay
  static long last_render = millis();
  while (millis() - last_render <= LHF_CONST_FRAME_TIME);
  last_render = millis();

  // Advance ringbuffer slot index, wrapping around
  if (++ringbuf_slot == LFH_FRAME_RINGBUF_SLOTS)
    ringbuf_slot = 0;
}