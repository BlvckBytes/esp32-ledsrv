#include <wsockh/responders/wsockh_get_frame_dur.h>

void wsockh_get_frame_dur(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_arg_numeric(client, vars_get_frame_dur(), 2);
}