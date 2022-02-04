#ifndef wsockh_set_frame_dur_h
#define wsockh_set_frame_dur_h

#include <ESPAsyncWebServer.h>
#include <inttypes.h>
#include <wsockh/wsockh_utils.h>
#include <variable_store.h>

void wsockh_set_frame_dur(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif