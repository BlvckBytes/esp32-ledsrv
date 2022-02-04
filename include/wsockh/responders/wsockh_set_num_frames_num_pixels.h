#ifndef wsockh_set_num_frames_num_pixels_h
#define wsockh_set_num_frames_num_pixels_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <variable_store.h>
#include <wsockh/wsockh_utils.h>
#include <led_frame_handler.h>

void wsockh_set_num_frames_num_pixels(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif