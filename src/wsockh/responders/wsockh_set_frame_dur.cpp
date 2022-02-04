#include <wsockh/responders/wsockh_set_frame_dur.h>

void wsockh_set_frame_dur(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  uint16_t frame_dur = 0;
  if (!wsockh_read_arg_16t(client, 0, data, len, &frame_dur)) return;

  vars_set_frame_dur(frame_dur);
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), FRAME_DUR_SET);
}