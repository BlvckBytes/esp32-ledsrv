#ifndef wsockh_get_frame_slots_h
#define wsockh_get_frame_slots_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <wsockh/wsockh_utils.h>
#include <lfh/lfh_frame_file.h>

void wsockh_get_frame_slots(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif