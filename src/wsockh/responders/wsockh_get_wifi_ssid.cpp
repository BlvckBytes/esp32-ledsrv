#include <wsockh/responders/wsockh_get_wifi_ssid.h>

void wsockh_get_wifi_ssid(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  const char* ssid = vars_get_wifi_ssid();
  wsockh_send_strings(client, &ssid, 1);
}