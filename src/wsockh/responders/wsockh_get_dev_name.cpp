#include <wsockh/responders/wsockh_get_dev_name.h>

void wsockh_get_dev_name(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  const char* dev_name = vars_get_dev_name();
  wsockh_send_strings(client, &dev_name, 1);
}