#include <wsockh/responders/wsockh_get_frame_cont.h>

wsockh_frame_cont_req_t *wsockh_alloc_frame_cont_req(AsyncWebSocketClient *client, uint16_t frame_index)
{
  wsockh_frame_cont_req_t *req = (wsockh_frame_cont_req_t *) malloc(sizeof(wsockh_frame_cont_req_t));
  req->client = client;
  req->frame_index = frame_index;
  return req;
}

void wsockh_dealloc_frame_cont_req(wsockh_frame_cont_req_t *data)
{
  free(data);
}

void wsockh_get_frame_cont(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  // Read frame_index
  uint16_t frame_index = 0;
  if (!wsockh_read_arg_16t(client, 0, data, len, &frame_index)) return;

  // Frame index out of range
  if (frame_index >= vars_get_num_frames())
  {
    wsockh_send_resp(client, ERR_INVAL_FRAME_IND);
    return;
  }

  // Pause frame processing
  lfh_pause_register([](void *arg)
  {
    wsockh_frame_cont_req_t *rd = (wsockh_frame_cont_req_t *) arg;

    // Read frame content into local buffer
    uint8_t frame_data_buf[vars_get_num_pixels() * 3] = { 0 };
    bool ret = lfh_frame_file_read(rd->frame_index, frame_data_buf);

    // Could not fetch the requested data
    if (!ret)
    {
      wsockh_send_resp(rd->client, ERR_CANNOT_READ_FRAME);
      return;
    }

    // Send frame data over to the client
    wsockh_send_resp(rd->client, SUCCESS_DATA_FOLLOWS, frame_data_buf, sizeof(frame_data_buf));

    // Free request
    wsockh_dealloc_frame_cont_req(rd);
  }, wsockh_alloc_frame_cont_req(client, frame_index));
}