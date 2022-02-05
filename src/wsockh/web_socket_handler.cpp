#include <wsockh/web_socket_handler.h>

AsyncWebSocket wsockh_ws(WSOCKH_PATH);

/*
============================================================================
                              Packet handling                              
============================================================================
*/

static wsockh_req_handler_t wsockh_sfo[256] = { 0x0 };
static wsockh_req_handler_t wsockh_mfo[256] = { 0x0 };

void wsockh_init_req_handler_lut() {
  // Single frame operations
  wsockh_sfo[SET_FRAME_DUR] = wsockh_set_frame_dur;
  wsockh_sfo[SET_NUM_FRAMES_NUM_PIXELS] = wsockh_set_num_frames_num_pixels;
  wsockh_sfo[SET_BRIGHTNESS] = wsockh_set_brightness;
  wsockh_sfo[SET_EVT_SUB] = wsockh_set_evt_sub;
  wsockh_sfo[GET_FRAME_DUR] = wsockh_get_frame_dur;
  wsockh_sfo[GET_NUM_FRAMES_NUM_PIXELS] = wsockh_get_num_frames_num_pixels;
  wsockh_sfo[GET_FRAME_SLOTS] = wsockh_get_frame_slots;
  wsockh_sfo[GET_FRAME_CONT] = wsockh_get_frame_cont;
  wsockh_sfo[GET_BRIGHTNESS] = wsockh_get_brightness;
  wsockh_sfo[GET_WIFI_SSID] = wsockh_get_wifi_ssid;
  wsockh_sfo[GET_SD_SIZE] = wsockh_get_sd_size;
  wsockh_sfo[GET_DEV_NAME] = wsockh_get_dev_name;
  wsockh_sfo[CMD_REBOOT] = wsockh_cmd_reboot;

  // Multi frame operations
  wsockh_mfo[SET_FRAME_CONT] = wsockh_set_frame_cont;
  wsockh_mfo[SET_WIFI_CRED] = wsockh_set_wifi_cred;
  wsockh_mfo[SET_DEV_NAME] = wsockh_set_dev_name;
}

/**
 * @brief Try to handle opcodes which bring data spanning accross multiple
 * packets that has been collected beforehand
 * 
 * @param client Request issuer
 * @param data Packet data
 * @param len Packet data length
 * 
 * @return true Request could be handled
 * @return false Request unknown
 */
bool wsockh_handle_multi_packet_req(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  wsockh_req_handler_t handler = wsockh_mfo[data[0]];

  // Handle multi frame request
  if (handler)
  {
    handler(client, data, len);
    return true;
  }

  // Not a multi frame request
  return false;
}

/**
 * @brief Try to handle opcodes that are single-packeted (small data)
 * 
 * @param client Request issuer
 * @param info Frame information
 * @param data Packet data
 * @param len Packet data length
 * 
 * @return true Request could be handled
 * @return false Request is larger and needs further attention
 */
bool wsockh_handle_single_packet_req(
  AsyncWebSocketClient *client,
  AwsFrameInfo *info,
  uint8_t *data,
  size_t len
)
{
  wsockh_req_handler_t handler = wsockh_sfo[data[0]];

  // Handle single frame request
  if (handler)
  {
    handler(client, data, len);
    return true;
  }

  // Not a single frame request
  return false;
}

/*
============================================================================
                                Data handling                               
============================================================================
*/

/**
 * @brief Handle incoming request data from the websocket
 * 
 * @param client Request issuer
 * @param info Information about the frame
 * @param data Received bytes
 * @param len Number of received bytes
 */
void wsockh_handle_data(
  AsyncWebSocketClient *client,
  AwsFrameInfo *info,
  uint8_t *data,
  size_t len
)
{
  static uint8_t msg_buf[WSOCKH_MSGBUF_SIZE];

  // Never accept non-binary data
  if (info->opcode != WS_BINARY)
  {
    wsockh_send_resp(client, ERR_TEXT_RECV);
    return;
  }

  // Never accept empty requests
  if (info->len == 0)
  {
    wsockh_send_resp(client, ERR_REQ_EMPTY);
    return;
  }

  // Don't act on this packet, it's no longer relevant
  if (wsockh_is_request_terminated(client, len))
    return;

  // Handle single frame requests
  if (wsockh_handle_single_packet_req(client, info, data, len)) {
    dbg_log("Single frame request opcode=%#04X handled!\n", data[0]);

    // Don't process this request any further, if it has any trailing data
    wsockh_terminate_request(client, info, len);
    return;
  }

  // Don't support fragmented messages
  if (!info->final)
  {
    dbg_log("Encountered unsupported fragmented request!\n");
    wsockh_send_resp(client, ERR_FRAG_REQ);

    // Don't process this request any further, if it has any trailing data
    wsockh_terminate_request(client, info, len);
    return;
  }

  // Message longer than internal buffer
  if (info->len > WSOCKH_MSGBUF_SIZE)
  {
    dbg_log("Encountered message exceeding internal buffer length!\n");
    wsockh_send_resp(client, ERR_REQ_TOO_LONG);

    // Don't process this request any further, if it has any trailing data
    wsockh_terminate_request(client, info, len);
    return;
  }

  // Collect packets into buffer
  memcpy(&msg_buf[info->index], data, len);

  // Continue to collect until last packet
  if (info->index + len != info->len)
    return;

  // Try to handle this big request now
  if (!wsockh_handle_multi_packet_req(client, msg_buf, info->len))
  {
    dbg_log("Received unknown opcode request!\n");
    wsockh_send_resp(client, ERR_UNKNOWN_OPCODE);

    // Don't process this request any further, if it has any trailing data
    wsockh_terminate_request(client, info, len);
    return;
  }
}

/*
============================================================================
                               Event handling                               
============================================================================
*/

void wsockh_ev_handler(
  AsyncWebSocket *server, AsyncWebSocketClient *client,
  AwsEventType type, void *arg, uint8_t *data, size_t len
)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      dbg_log("Websocket client %u connected!\n", client->id());
      break;

    case WS_EVT_DISCONNECT:
      dbg_log("Websocket client %u disconnected!\n", client->id());

      // Remove client from event handler, if was registered
      evh_remove_client(client->id());
      break;

    case WS_EVT_ERROR:
      dbg_log("Websocket client %u experienced error %u): %s\n", client->id(), *((uint16_t*) arg), (char*) data);
      break;

    case WS_EVT_PONG:
      dbg_log("Websocket client %u ponged (len: %u): %s\n", client->id(), len, (len) ? (char*) data : "");
      break;

    case WS_EVT_DATA:
      wsockh_handle_data(client, (AwsFrameInfo*) arg, data, len);
      break;
    
    default:
      dbg_log("Unknown websocket event occurred!\n");
      break;
  }
}

/*
============================================================================
                            Basic socket control                            
============================================================================
*/

void wsockh_init()
{
  wsockh_init_req_handler_lut();
  evh_set_notifier(&wsockh_ws, wsockh_notify_client);
  wsockh_ws.onEvent(wsockh_ev_handler);
  wsrvh_register_handler(&wsockh_ws);
}

void wsockh_cleanup()
{
  wsockh_ws.cleanupClients();
}
