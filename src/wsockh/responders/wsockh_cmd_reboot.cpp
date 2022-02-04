#include <wsockh/responders/wsockh_cmd_reboot.h>

void wsockh_cmd_reboot(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  dbg_log("Restarting device in %dms!\n", WSOCKH_REB_DEL);
  rbh_request_schedule(WSOCKH_REB_DEL );
}