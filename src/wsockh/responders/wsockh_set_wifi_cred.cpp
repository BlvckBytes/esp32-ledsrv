#include <wsockh/responders/wsockh_set_wifi_cred.h>

void wsockh_set_wifi_cred(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  static char cred_buf[WSOCKH_STRARGBUF_SIZE][VARS_STRVALBUF_SIZE];

  if (!wsockh_read_strings(client, 0, data, len, 2, cred_buf)) return;
  vars_set_wifi_ssid(cred_buf[0]);
  vars_set_wifi_pass(cred_buf[1]);
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), WIFI_CRED_SET);
}