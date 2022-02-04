#ifndef wsockh_get_dev_name_h
#define wsockh_get_dev_name_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <variable_store.h>
#include <wsockh/wsockh_utils.h>

void wsockh_get_dev_name(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif