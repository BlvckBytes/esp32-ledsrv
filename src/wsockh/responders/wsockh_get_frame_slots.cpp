#include <wsockh/responders/wsockh_get_frame_slots.h>

void wsockh_get_frame_slots(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_arg_numeric(client, lfh_frame_file_get_slots(), 2);
}