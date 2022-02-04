#include <wsockh/responders/wsockh_set_dev_name.h>

void wsockh_set_dev_name(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  static char dev_name_buf[1][VARS_STRVALBUF_SIZE];
  if (!wsockh_read_strings(client, 0, data, len, 1, dev_name_buf)) return;

  // Check if there are invalid characters inside the SSID
  for (int i = 0; i < strlen(dev_name_buf[0]); i++)
  {
    char c = dev_name_buf[0][i];
    if (c < 32 || c > 126)
    {
      wsockh_send_resp(client, ERR_INVAL_DEV_NAME);
      return;
    }
  }

  vars_set_dev_name(dev_name_buf[0]);
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), DEV_NAME_SET);
  return;
}