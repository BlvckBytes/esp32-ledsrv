#include <wsockh/responders/wsockh_get_brightness.h>

void wsockh_get_brightness(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_arg_numeric(client, vars_get_brightness(), 2);
}