#ifndef wsockh_get_frame_dur_h
#define wsockh_get_frame_dur_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <wsockh/wsockh_utils.h>
#include <variable_store.h>

void wsockh_get_frame_dur(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif