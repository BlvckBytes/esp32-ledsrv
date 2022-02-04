#include <wsockh/responders/wsockh_get_sd_size.h>

void wsockh_get_sd_size(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_arg_numeric(client, sdh_get_total_size_mb(), 4);
}