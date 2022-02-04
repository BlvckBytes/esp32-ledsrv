#include <wsockh/responders/wsockh_set_num_frames_num_pixels.h>

void wsockh_set_num_frames_num_pixels(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  // Read num_frames
  uint16_t num_frames = 0;
  if (!wsockh_read_arg_16t(client, 0, data, len, &num_frames)) return;

  // Cannot accept more frames than the slots allow
  if (num_frames > lfh_get_frame_slots())
  {
    wsockh_send_resp(client, ERR_TOO_MANY_FRAMES);
    return;
  }

  // Read num_pixels
  uint16_t num_pixels = 0;
  if (!wsockh_read_arg_16t(client, 2, data, len, &num_pixels)) return;

  // More pixels than capable of handling
  if (num_pixels > lfh_get_max_num_pixels())
  {
    wsockh_send_resp(client, ERR_TOO_MANY_PIXELS);
    return;
  }

  // TODO: Rework this part
  lfh_deinit();
  vars_set_num_frames(num_frames);
  vars_set_num_pixels(num_pixels);
  lfh_init_file();
  lfh_init();

  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), NUM_FRAMES_NUM_PIXELS_SET);
}