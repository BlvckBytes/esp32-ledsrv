#include <wsockh/responders/wsockh_set_num_frames_num_pixels.h>

wsockh_set_num_frames_num_pixels_t *wsockh_alloc_wsockh_set_num_frames_num_pixels_t(uint16_t num_frames, uint16_t num_pixels)
{
  wsockh_set_num_frames_num_pixels_t *data = (wsockh_set_num_frames_num_pixels_t *) malloc(sizeof(wsockh_set_num_frames_num_pixels_t));

  data->num_frames = num_frames;
  data->num_pixels = num_pixels;

  return data;
}

void wsockh_dealloc_wsockh_set_num_frames_num_pixels_t(wsockh_set_num_frames_num_pixels_t *data)
{
  free(data);
}

/**
 * @brief Deallocate a set num_frames and num_pixels structure
 * 
 * @param data Data to free up
 */
void wsockh_dealloc_wsockh_set_num_frames_num_pixels_t(wsockh_set_num_frames_num_pixels_t *data);

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
  if (num_frames > lfh_frame_file_get_slots())
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

  lfh_pause_register([](void *arg){
    wsockh_set_num_frames_num_pixels_t *rd = (wsockh_set_num_frames_num_pixels_t *) arg;

    vars_set_num_frames(rd->num_frames);
    vars_set_num_pixels(rd->num_pixels);

    wsockh_dealloc_wsockh_set_num_frames_num_pixels_t(rd);
  }, wsockh_alloc_wsockh_set_num_frames_num_pixels_t(num_frames, num_pixels));

  wsockh_send_resp(client, SUCCESS_NO_DATA);
  evh_fire_event(client->id(), NUM_FRAMES_NUM_PIXELS_SET);
}