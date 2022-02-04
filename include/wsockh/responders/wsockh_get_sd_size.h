#ifndef wsockh_get_sd_size_h
#define wsockh_get_sd_size_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <sd_handler.h>
#include <wsockh/wsockh_utils.h>

void wsockh_get_sd_size(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif