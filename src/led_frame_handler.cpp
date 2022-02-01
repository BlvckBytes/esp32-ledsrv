#include <led_frame_handler.h>

/*
============================================================================
                               Basic control                                
============================================================================
*/

static File lfh_framebuf;
static long lfh_last_render;
static uint32_t lfh_current_frame;

void lfh_init()
{
  sdh_open_frames_file("r", &lfh_framebuf);
  lfh_last_render = millis();
  lfh_current_frame = 0;
}

void lfh_deinit()
{
  if (lfh_framebuf) lfh_framebuf.close();
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

  // Create empty buffer (all pixels off)
  uint16_t fsz = lfh_get_frame_size();
  uint8_t empty_buf[fsz] = { 0 };

  // Write empty frames to it
  for (uint16_t i = 0; i < fsz; i++)
  {
    if (!handle.seek(i * fsz))
    {
      dbg_log("Could not seek next write position!\n");
      return false;
    }

    // Write empty frame
    handle.write(empty_buf, fsz);
  }

  dbg_log(
    "Initialized new frames file for num_pixels=%" PRIu16 " and num_frames=%" PRIu16 " !\n",
    vars_get_num_pixels(),
    lfh_get_num_frames_capped()
  );

  // Close and thus flush
  handle.close();
  return true;
}

// TODO: Implement!
void lfh_resize_file()
{
  // INFO: This should either add or remove frames from the file, based on the current capped number of frames
}

bool lfh_write_frame(uint16_t frame_index, uint8_t *frame_data)
{
  File handle;

  // Open file for write access
  sdh_open_frames_file("w", &handle);

  // Could not open file
  if (!handle) return false;

  // Go to frame start
  if (!handle.seek(lfh_get_frame_size() * frame_index))
  {
    dbg_log("Frame file wasn't large enough!\n");

    // Re-try after trying to create a new file
    if (lfh_init_file())
      lfh_write_frame(frame_index, frame_data);
  }

  // Write data
  handle.write(frame_data, lfh_get_frame_size());
  handle.close();
  return true;
}

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

uint16_t lfh_get_frame_slots()
{
  return MAX_FRAMES;
}

uint16_t lfh_get_num_frames_capped()
{
  return min(vars_get_num_frames(), lfh_get_frame_slots());
}

uint16_t lfh_get_frame_size()
{
  return vars_get_num_pixels() * 3;
}

/*
============================================================================
                              Frame handling                                
============================================================================
*/

/**
 * @brief Draw a frame to the pixels
 * 
 * @param frame_buf Frame buffer containing pixel color data
 * @param frame_size Size of one frame in bytes
 */
void lfh_draw_frame(uint8_t *frame_buf, uint32_t frame_size)
{
  // Each pixel has to have three color components
  if (frame_size % 3 != 0)
  {
    dbg_log("Invalid frame_size of %" PRIu32 " provided!\n", frame_size);
    return;
  }

  dbg_log("Drawing frame %" PRIu32 "! :)\n", lfh_current_frame);

  for (int i = 0; i < frame_size; i += 3)
    dbg_log("(%" PRIu8 ", %" PRIu8 ", %" PRIu8 ")\n", frame_buf[i], frame_buf[i + 1], frame_buf[i + 2]);
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

  uint8_t frame_buf[frame_size] = { 0 };

  // File has not enough bytes remaining, thus no frame info available from here on onwards, reset
  if (lfh_framebuf.available() < frame_size)
  {
    dbg_log("Not enough LED frame data remaining, resetting!\n");
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
  if (++lfh_current_frame == lfh_get_num_frames_capped())
    lfh_current_frame = 0;
}