#ifndef wsockh_set_evt_sub_h
#define wsockh_set_evt_sub_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <wsockh/wsockh_utils.h>
#include <event_handler.h>

void wsockh_set_evt_sub(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif