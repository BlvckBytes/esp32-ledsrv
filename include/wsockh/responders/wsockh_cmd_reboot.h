#ifndef wsockh_cmd_reboot_h
#define wsockh_cmd_reboot_h

#include <AsyncWebSocket.h>
#include <wsockh/wsockh_utils.h>
#include <inttypes.h>
#include <reboot_handler.h>
#include <dbg_log.h>

// Reboot delay between command and actual reboot
#define WSOCKH_REB_DEL 5000

void wsockh_cmd_reboot(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif