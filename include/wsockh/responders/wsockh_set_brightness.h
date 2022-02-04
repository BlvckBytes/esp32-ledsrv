#ifndef wsockh_set_brightness_h
#define wsockh_set_brightness_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <variable_store.h>
#include <wsockh/wsockh_utils.h>

void wsockh_set_brightness(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif