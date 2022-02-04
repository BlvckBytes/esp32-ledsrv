#include <wsockh/responders/wsockh_set_brightness.h>

void wsockh_set_brightness(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  // <opcode><brightness uint8_t>
  if (len != 2)
  {
    wsockh_send_resp(client, ERR_ARGS_MISMATCH);
    return;
  }

  vars_set_brightness(data[1]);
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), BRIGHTNESS_SET);
}