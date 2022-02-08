#include <lfh/led_frame_handler.h>

/*
============================================================================
                               Basic control                                
============================================================================
*/

static uint8_t *lfh_frame_ringbuf;

static uint64_t lfh_frame_ringbuf_index_w, lfh_frame_ringbuf_index_r;

/**
 * @brief Allocate the frame ringbuffer based on the
 * frame number of blocks
 */
void lfh_frame_ringbuf_alloc()
{
  size_t total_size = 
    LFH_FRAME_RINGBUF_SLOTS // This many times
    * vars_get_num_pixels() * 3; // This many frames

  lfh_frame_ringbuf = (uint8_t *) calloc(1, total_size);
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
  lfh_frame_file_open();

  // Allocate and initialize frame ringbuffer
  lfh_frame_ringbuf_alloc();

  // Initialize RMT
  lfh_rmt_init();

  // Initially fill the ringbuffer with frames so
  // write has an advantage of read
  lfh_fill_frame_ringbuf();
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

bool lfh_read_frames(uint64_t ringbuf_index, uint64_t num_frames)
{
  static uint64_t frame_index = 0;

  bool res = lfh_frame_file_read(
    frame_index % vars_get_num_frames(),
    num_frames,
    &lfh_frame_ringbuf[
      (ringbuf_index % LFH_FRAME_RINGBUF_SLOTS) * vars_get_num_pixels() * 3
    ]
  );

  if (res)
    frame_index += num_frames;

  return res;
}

/**
 * @brief Read needed next frames task
 * @param arg Unused task argument (keep NULL)
 */
void lfh_read_frames_task(void *arg)
{
  // Make the write index lead the read index by slots / 2
  uint64_t num_frames = max((LFH_FRAME_RINGBUF_SLOTS / 2) - (int) (lfh_frame_ringbuf_index_w - lfh_frame_ringbuf_index_r), 0);
  if (lfh_read_frames(lfh_frame_ringbuf_index_w, num_frames))
    lfh_frame_ringbuf_index_w += num_frames;

  vTaskDelete(NULL);
}

/**
 * @brief Write specific framebuffer frame on data pin
 * @param arg Framebuffer index of target frame
 */
void lfh_write_frame_task(void *arg)
{
  // Copy frame from ringbuffer into RMT data buffer by transforming it to bits
  lfh_rmt_copy_frame(&lfh_frame_ringbuf[
    (*((uint64_t *) arg) % LFH_FRAME_RINGBUF_SLOTS) * vars_get_num_pixels() * 3
  ], vars_get_num_pixels() * 3);

  // Write out RMT data
  lfh_rmt_write_items(true);
  vTaskDelete(NULL);
}

void lfh_fill_frame_ringbuf()
{
  // Cancel previous task
  static TaskHandle_t *read_frame_handle = NULL;
  if (read_frame_handle) vTaskDelete(read_frame_handle);

  // Request ring buffer fill
  xTaskCreatePinnedToCore(
    lfh_read_frames_task,
    "read_frames",
    2048,
    read_frame_handle,
    configMAX_PRIORITIES - 1,
    read_frame_handle,
    LFH_FILE_CORE
  );
}

/**
 * @brief Write the next frame from the ringbuffer onto the line
 */
void lfh_write_frame_ringbuf()
{
  // Allocate write index pointer initially
  static uint64_t *write_index = NULL;
  if (write_index == NULL)
    write_index = (uint64_t *) malloc(sizeof(uint64_t));

  // Cancel previous task
  static TaskHandle_t *write_frame_handle = NULL;
  if (write_frame_handle) vTaskDelete(write_frame_handle);

  // Request frame draw
  *write_index = lfh_frame_ringbuf_index_r++;
  xTaskCreatePinnedToCore(
    lfh_write_frame_task,
    "write_frame",
    2048,
    write_index,
    configMAX_PRIORITIES - 1,
    write_frame_handle,
    LFH_DRAW_CORE
  );
}

void lfh_handle_frame()
{
  // Inter-frame delay
  static long last_render = millis();
  if (millis() - last_render <= LHF_CONST_FRAME_TIME)
    return;

  // Ringbuffer safety
  if (lfh_frame_ringbuf_index_w <= lfh_frame_ringbuf_index_r)
    return;

  // Write out frame and read next frames into ringbuffer
  lfh_write_frame_ringbuf();
  lfh_fill_frame_ringbuf();

  last_render = millis();
}