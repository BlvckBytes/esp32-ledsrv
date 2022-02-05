#ifndef wsockh_set_num_frames_num_pixels_h
#define wsockh_set_num_frames_num_pixels_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <variable_store.h>
#include <wsockh/wsockh_utils.h>
#include <lfh/lfh_pause.h>
#include <lfh/lfh_frame_file.h>
#include <lfh/led_frame_handler.h>

typedef struct
{
  uint16_t num_frames;
  uint16_t num_pixels;
} wsockh_set_num_frames_num_pixels_t;

/**
 * @brief Allocates a set num_frames and num_pixels struct on the heap
 * 
 * @param num_frames Number of frames to apply
 * @param num_pixels Number of pixels to apply
 * @return wsockh_set_num_frames_num_pixels_t 
 */
wsockh_set_num_frames_num_pixels_t *wsockh_alloc_wsockh_set_num_frames_num_pixels_t(uint16_t num_frames, uint16_t num_pixels);

/**
 * @brief Deallocate a set num_frames and num_pixels structure
 * 
 * @param data Data to free up
 */
void wsockh_dealloc_wsockh_set_num_frames_num_pixels_t(wsockh_set_num_frames_num_pixels_t *data);

void wsockh_set_num_frames_num_pixels(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif