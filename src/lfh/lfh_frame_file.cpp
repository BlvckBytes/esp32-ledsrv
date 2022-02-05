#include <lfh/lfh_frame_file.h>

static File lfh_frame_file;

bool lfh_frame_file_reset()
{
  // Check if file is in use
  if (lfh_frame_file)
  {
    dbg_log("Frame file is currently in use, cannot reset!\n");
    return false;
  }

  // Delete old file
  if (!sdh_delete_frames_file())
  {
    dbg_log("Could not delete the old frames file!\n");
    return false;
  }

  // Open file for write access
  sdh_open_frames_file("w", &lfh_frame_file);

  // Could not open file
  if (!lfh_frame_file)
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
      lfh_frame_file.write(empty_pixel, sizeof(empty_pixel));
  }

  dbg_log(
    "Initialized new frames file for num_pixels=%" PRIu16 " and num_frames=%" PRIu16 " !\n",
    vars_get_num_pixels(),
    vars_get_num_frames()
  );

  // Close resource
  lfh_frame_file.close();

  // Process pause queue
  if (lfh_pause_check())
    lfh_pause_call((lfh_resume_t) lfh_frame_file_open);

  return true;
}

bool lfh_frame_file_write(uint16_t frame_index, uint8_t *frame_data)
{
  // Check if file is in use
  if (lfh_frame_file)
  {
    dbg_log("Frame file is currently in use, cannot reset!\n");
    return false;
  }

  // Open for patching
  sdh_open_frames_file("r+", &lfh_frame_file);

  // Could not open file
  if (!lfh_frame_file) return false;

  // Go to frame start
  if (!lfh_frame_file.seek(vars_get_num_pixels() * 3 * frame_index))
  {
    dbg_log("Frame file wasn't large enough!\n");

    // Re-try after trying to create a new file
    if (lfh_frame_file_reset())
      return lfh_frame_file_write(frame_index, frame_data);

    return false;
  }

  // Write buffer into file
  // WARNING: Test if this works
  lfh_frame_file.write(frame_data, vars_get_num_pixels() * 3);

  // Close file
  lfh_frame_file.close();

  dbg_log(
    "Overwrote frame %" PRIu16 " at offset %" PRIu32 "!\n",
    frame_index,
    vars_get_num_pixels() * 3 * frame_index
  );

  // Process pause queue
  if (lfh_pause_check())
    lfh_pause_call((lfh_resume_t) lfh_frame_file_open);

  return true;
}

bool lfh_frame_file_read(uint16_t frame_index, uint8_t *out_buf)
{
  // Persistent buffer not available
  if (!lfh_frame_file) return false;

  // Go to start of current frame's block
  if (!lfh_frame_file.seek(vars_get_num_pixels() * 3 * frame_index))
  {
    dbg_log("LED frame %" PRIu32 " not available!\n", frame_index);
    return false;
  }

  // Read into buffer using as many 512 byte blocks as possible,
  // then read the remaining bytes at last iteration
  uint16_t frame_size = vars_get_num_pixels() * 3;
  uint16_t num_blocks = ceil(((double) frame_size) / 512.0), buf_offs = 0;
  for (int i = 0; i < num_blocks; i++)
  {
    uint16_t curr_sz = i == num_blocks - 1 ? frame_size % 512 : 512;
    lfh_frame_file.readBytes((char *) &out_buf[buf_offs], curr_sz);
    buf_offs += curr_sz;
  }

  // Process pause queue
  if (lfh_pause_check())
  {
    lfh_frame_file_close();
    lfh_pause_call((lfh_resume_t) lfh_frame_file_open);
  }

  // Read successfully
  return true;
}

uint16_t lfh_frame_file_get_slots()
{
  return LFH_MAX_FRAMES;
}

bool lfh_frame_file_open()
{
  // Already open
  if (lfh_frame_file) return false;
  return sdh_open_frames_file("r", &lfh_frame_file);
}

bool lfh_frame_file_close()
{
  // Not open
  if (!lfh_frame_file) return false;
  lfh_frame_file.close();
  return true;
}