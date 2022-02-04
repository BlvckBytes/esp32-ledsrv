#include <wsockh/responders/wsockh_get_num_frames_num_pixels.h>

void wsockh_get_num_frames_num_pixels(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_send_arg_numeric(
    client,
    (vars_get_num_frames() << 16) | vars_get_num_pixels(),
    4
  );
}