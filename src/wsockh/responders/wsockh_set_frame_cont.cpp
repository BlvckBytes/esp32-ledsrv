#include <wsockh/responders/wsockh_set_frame_cont.h>

wsockh_frame_cont_set_t *wsockh_alloc_wsockh_frame_cont_set(
  AsyncWebSocketClient *client,
  uint16_t frame_index,
  uint8_t *content,
  uint16_t content_len
)
{
  wsockh_frame_cont_set_t *req = (wsockh_frame_cont_set_t *) malloc(sizeof(wsockh_frame_cont_set_t));
  req->client = client;
  req->frame_index = frame_index;

  req->content = (uint8_t *) malloc(content_len);
  memcpy(req->content, content, content_len);

  return req;
}

void wsockh_dealloc_wsockh_frame_cont_set(wsockh_frame_cont_set_t *data)
{
  free(data->content);
  data->content = NULL;
  free(data);
}

void wsockh_set_frame_cont(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  // Read frame index arg
  uint16_t frame_index = 0;
  if (!wsockh_read_arg_16t(client, 0, data, len, &frame_index)) return;

  // Frame index out of range
  if (frame_index >= lfh_get_frame_slots())
  {
    wsockh_send_resp(client, ERR_INVAL_FRAME_IND);
    return;
  }

  // Check if full frame is provided (frame_size + opcode + uint16_t arg)
  uint16_t frame_size = lfh_get_frame_size();
  if (len != frame_size + 1 + 2)
  {
    wsockh_send_resp(client, ERR_NUM_PIXEL_MISMATCH);
    return;
  }

  // Create timeout to resume the LFH
  static int toh_handle = -1;
  if (!toh_is_active(toh_handle))
  {
    toh_handle = toh_create_timeout(WSOCKH_SET_FRAME_TIMEOUT, lfh_resume);
  }

  // Pause frame processing
  lfh_pause([](void *arg) {
    wsockh_frame_cont_set_t *rd = (wsockh_frame_cont_set_t *) arg;

    // Could not write into file
    if (!lfh_write_frame(rd->frame_index, rd->content))
    {
      wsockh_send_resp(rd->client, ERR_NO_SD_ACC);
      return;
    }

    wsockh_send_resp(rd->client, SUCCESS_NO_DATA);
    wsockh_dealloc_wsockh_frame_cont_set(rd);
  }, wsockh_alloc_wsockh_frame_cont_set(client, frame_index, &data[3], frame_size));

  // Re-set timeout on every frame
  toh_reset(toh_handle);
}