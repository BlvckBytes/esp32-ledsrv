#ifndef lfh_frame_file_h
#define lfh_frame_file_h

#include <inttypes.h>
#include <lfh/lfh_pause.h>
#include <sd_handler.h>
#include <dbg_log.h>
#include <variable_store.h>
#include <FS.h>

#define LFH_MAX_FRAMES 180

/**
 * @brief Reset the frame file back to a default empty state
 */
bool lfh_frame_file_reset();

/**
 * @brief Write a frame to the frames file at it's calculated offset
 * 
 * @param frame_index Index of the frame to write
 * @param frame_data Data of the frame
 */
bool lfh_frame_file_write(uint16_t frame_index, uint8_t *frame_data);

/**
 * @brief Read a frame from the frames file into a buffer
 * 
 * @param frame_index Index of the frame to read
 * @param num_frames Number of frames to read at once
 * @param out_buf Buffer for the data of the frame
 */
bool lfh_frame_file_read(uint16_t frame_index, uint16_t num_frames, uint8_t *out_buf);

/**
 * @brief Get the maximum number of frame slots the file can store
 */
uint16_t lfh_frame_file_get_slots();

/**
 * @brief Open the frame file on the internal handle
 */
bool lfh_frame_file_open();

/**
 * @brief Close the internal file handle
 */
bool lfh_frame_file_close();

#endif