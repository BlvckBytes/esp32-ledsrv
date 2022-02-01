#include <web_socket_handler.h>

AsyncWebSocket wsockh_ws(WSOCKH_PATH);

/*
============================================================================
                         Request remainder handling                         
============================================================================
*/

// Request remainder buffer
static AsyncWebSocketRequestRemainder wsockh_request_remainders[WSOCKH_REQUEST_REMAINDERS_LEN];
static uint8_t wsockh_request_remainder_pointer = 0;

/**
 * @brief Check if a request is terminated already and thus can be ignored
 * 
 * @param client Origin of the target request
 * @param read_len Length of current packet
 * 
 * @return true Request can be ignored
 * @return false Request is valid and needs processing
 */
bool wsockh_is_request_terminated(AsyncWebSocketClient *client, size_t read_len)
{
  for (int i = 0; i < WSOCKH_REQUEST_REMAINDERS_LEN; i++)
  {
    AsyncWebSocketRequestRemainder *tar = &wsockh_request_remainders[i];

    // Search on
    if (tar->client != client) continue;

    // No remainder
    if (tar->remainder <= 0) continue;

    // Request is ignored
    tar->remainder -= read_len;
    dbg_log("Ignoring terminated request from client %" PRIu32 " remaining %" PRIu64 "!\n", client->id(), tar->remainder);
    return true;
  }

  // No entry available, not terminated
  return false;
}

/**
 * @brief Terminate a request by marking the remaining bytes that are to be received as ignoreable
 * 
 * @param client Origin of the target request
 * @param info Frame info to calculate remaining size
 * @param read_len Length of current packet
 */
void wsockh_terminate_request(AsyncWebSocketClient *client, AwsFrameInfo *info, size_t read_len)
{
  uint64_t remainder = info->len - read_len;

  // Nothing to ignore
  if (remainder <= 0) return;

  // Register remainder
  AsyncWebSocketRequestRemainder rem = { client, remainder };

  // Add to ringbuffer
  wsockh_request_remainders[wsockh_request_remainder_pointer] = rem;
  dbg_log("Writing request ignore at %" PRIu8 " for client %" PRIu32 " remaining %" PRIu64 "!\n", wsockh_request_remainder_pointer, client->id(), remainder);
  wsockh_request_remainder_pointer = (wsockh_request_remainder_pointer + 1) % WSOCKH_REQUEST_REMAINDERS_LEN;
}

/*
============================================================================
                            Responding Utilities                            
============================================================================
*/

/**
 * @brief Send a binary response to the client while adding a leading response code
 * 
 * @param client Recipient of the message
 * @param code Result code to append as head
 * @param bytes Argument bytes
 * @param num_bytes Number of bytes to send
 */
void wsockh_send_resp(
  AsyncWebSocketClient *client,
  CommResultCode code,
  uint8_t *bytes = NULL,
  int num_bytes = 0
)
{
  // Create buffer with first element being the code
  uint8_t buf[num_bytes + 1] = {};
  buf[0] = code;

  // Copy over args into buffer
  if (num_bytes != 0)
    memcpy(&buf[1], bytes, num_bytes);

  // Send opcode with args
  client->binary(buf, num_bytes + 1);
}

/**
 * @brief Send a numeric argument of variable size to the client
 * 
 * @param client Client to send argument to
 * @param value Numeric value
 * @param num_bytes How many bytes to mask out and send
 * @param result_code Result code to add as first byte
 * @param prefix Pointer to a prefixing byte that gets inserted
 * after result_code, null if not needed
 */
void wsockh_send_arg_numeric(
  AsyncWebSocketClient *client,
  uint64_t value,
  uint8_t num_bytes,
  CommResultCode result_code = SUCCESS_DATA_FOLLOWS,
  uint8_t *prefix = 0
)
{
  uint8_t pref_offs = prefix == 0 ? 0 : 1;
  uint8_t data_buf[num_bytes + pref_offs];

  // Append prefix if applicable
  if (prefix) data_buf[0] = *prefix;

  // Mask out individual bytes
  for (uint8_t i = 0; i < sizeof(data_buf); i++)
  {
    uint8_t curr_val = (value >> (i * 8)) & 0xFF;
    data_buf[sizeof(data_buf) - 1 - i] = curr_val;
  }

  // Send data with proper resultcode
  wsockh_send_resp(client, result_code, data_buf, sizeof(data_buf));
}

/**
 * @brief Send strings as an answer to the client
 * 
 * @param client Client to send strings to
 * @param strings Strings to send
 * @param num_strings Number of strings in total
 * @param result_code Result code to add as first byte
 * @param prefix Pointer to a prefixing byte that gets inserted
 * after result_code, null if not needed
 */
void wsockh_send_strings(
  AsyncWebSocketClient *client,
  const char **strings,
  size_t num_strings,
  CommResultCode result_code = SUCCESS_DATA_FOLLOWS,
  uint8_t *prefix = 0
)
{
  // Figure out total buffer length, including <NULL> chars
  uint8_t pref_offs = prefix == 0 ? 0 : 1;
  size_t total_length = 0;
  for (int i = 0; i < num_strings; i++)
    total_length += strlen(strings[i]) + 1;

  // Data buffer
  uint8_t data_buf[total_length + pref_offs];
  size_t data_buf_pointer = pref_offs;

  // Iterate strings
  for (int i = 0; i < num_strings; i++) {
    const char *curr_str = strings[i];

    // Iterate chars of string
    for (int j = 0; j < strlen(curr_str); j++)
      data_buf[data_buf_pointer++] = curr_str[j];

    // Add null terminator
    data_buf[data_buf_pointer++] = 0;
  }

  // Append prefix if applicable
  if (prefix) data_buf[0] = *prefix;

  // Send data with proper resultcode
  wsockh_send_resp(client, result_code, data_buf, sizeof(data_buf));
}

/*
============================================================================
                            Requesting Utilities                            
============================================================================
*/

/**
 * @brief Read a 16 bit argument from data into the provided buffer
 * 
 * @param client Client to send arg mismatch errors to
 * @param offset Offset from the beginning of data, without the OpCode
 * @param data Data to read from
 * @param data_len Length of data provided
 * @param out Output buffer
 * 
 * @return true Successfully parsed, value is now in buffer
 * @return false Could not parse, buffer is undefined
 */
bool wsockh_read_arg_16t(
  AsyncWebSocketClient *client,
  uint16_t offset,
  uint8_t *data,
  size_t data_len,
  uint16_t *out
)
{
  // Cannot be less than offset + 16b + opcode
  if (data_len < offset + 2 + 1)
  {
    wsockh_send_resp(client, ERR_ARGS_MISMATCH);
    return false;
  }

  // Get both bytes
  uint8_t msB = data[1 + offset];
  uint8_t lsB = data[1 + offset + 1];

  // Combine and write into output buffer
  *out = (msB << 8) | lsB;
  return true;
}

/**
 * @brief Read a number of zero terminated strings from data into the out buffer
 * 
 * @param client Client to send arg mismatch errors to
 * @param offset Offset from the beginning of data, without the OpCode
 * @param data Data to read from
 * @param data_len Length of data provided
 * @param num_strings Number of strings to read
 * @param out Output buffer
 * 
 * @return true Successfully parsed, strings are now in buffer
 * @return false Could not parse, buffer is undefined
 */
bool wsockh_read_strings(
  AsyncWebSocketClient *client,
  uint16_t offset,
  uint8_t *data,
  size_t data_len,
  size_t num_strings,
  char out[WSOCKH_STRARGBUF_SIZE][VARS_STRVALBUF_SIZE]
)
{
  // Cannot be less than offset + opcode + at least one char + <NULL>
  if (data_len < offset + 1 + 1 + 1)
  {
    wsockh_send_resp(client, ERR_ARGS_MISMATCH);
    return false;
  }

  // Trying to read more string args than the buffer can hold
  if (num_strings > WSOCKH_STRARGBUF_SIZE)
  {
    dbg_log("Could not read strings from request, num_strings exceeded buffer size!\n");
    wsockh_send_resp(client, ERR_TOO_MANY_STRARGS);
    return false;
  }

  uint16_t curr_offs = offset + 1;
  uint8_t strings_found = 0;
  for (size_t i = 0; i < num_strings; i++)
  {
    // Collect until <NULL>
    for (size_t j = curr_offs; j < data_len; j++)
    {
      // String argument too long
      if (j - curr_offs == VARS_STRVALBUF_SIZE)
      {
        dbg_log("String argument too long!\n");
        wsockh_send_resp(client, ERR_STR_TOO_LONG);
        return false;
      }

      out[i][j - curr_offs] = data[j];

      // String terminated
      if (data[j] == 0)
      {
        curr_offs = j + 1;
        strings_found++;
        break;
      }

      // Unterminated string encountered
      else if (j == data_len - 1)
      {
        dbg_log("Unterminated string encountered at index=%" PRIu32 " of message!\n", j);
        wsockh_send_resp(client, ERR_STRING_UNTERMINATED);
        return false;
      }
    }
  }

  // Not enough strings provided as args
  if (strings_found != num_strings)
  {
    dbg_log("Not enough string args found!\n");
    wsockh_send_resp(client, ERR_ARGS_MISMATCH);
    return false;
  }

  // Enough terminated strings found and written into out buffer
  return true;
}

/*
============================================================================
                              Packet handling                              
============================================================================
*/

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
  // String argument buffer
  static char strarg_buf[WSOCKH_STRARGBUF_SIZE][VARS_STRVALBUF_SIZE] = { { 0 } };

  // Buffer for reading 16 bit arguments into
  static uint16_t arg_16t_buf;

  uint32_t client_id = client->id();

  switch (data[0])
  {
    case SET_FRAME_CONT:
      // Cancel frame processing
      lfh_deinit();

      // Read frame index arg
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;

      // Frame index out of range
      if (arg_16t_buf >= lfh_get_num_frames_capped())
      {
        wsockh_send_resp(client, ERR_INVAL_FRAME_IND);
        return true;
      }

      // Check if full frame is provided (frame_size + opcode + uint16_t arg)
      if (len != lfh_get_frame_size() + 1 + 2)
      {
        wsockh_send_resp(client, ERR_NUM_PIXEL_MISMATCH);
        return true;
      }

      // Could not write into file
      if (!lfh_write_frame(arg_16t_buf, &data[3]))
      {
        wsockh_send_resp(client, ERR_NO_SD_ACC);
        return true;
      }

      // Restart frame processing
      lfh_init();
      return true;

    case SET_WIFI_CRED:
      if (!wsockh_read_strings(client, 0, data, len, 2, strarg_buf)) return true;
      vars_set_wifi_ssid(strarg_buf[0]);
      vars_set_wifi_pass(strarg_buf[1]);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, WIFI_CRED_SET);
      return true;

    case SET_DEV_NAME:
      if (!wsockh_read_strings(client, 0, data, len, 1, strarg_buf)) return true;

      // Check if there are invalid characters inside the SSID
      for (int i = 0; i < strlen(strarg_buf[0]); i++)
      {
        char c = strarg_buf[0][i];
        if (c < 32 || c > 126)
        {
          wsockh_send_resp(client, ERR_INVAL_DEV_NAME);
          return true;
        }
      }

      vars_set_dev_name(strarg_buf[0]);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, DEV_NAME_SET);
      return true;
  
    default:
      return false;
  }
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
  // Buffer for reading 16 bit arguments into
  static uint16_t arg_16t_buf;

  // Buffer for holding result string arguments from function returns
  const char *str_arg_buf;

  uint32_t client_id = client->id();

  switch (data[0])
  {
    case SET_FRAME_DUR:
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;
      vars_set_frame_dur(arg_16t_buf);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, FRAME_DUR_SET);
      return true;

    case SET_NUM_FRAMES:
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;

      // Cannot accept more frames than the cap allows
      if (arg_16t_buf > lfh_get_num_frames_capped())
      {
        wsockh_send_resp(client, ERR_TOO_MANY_FRAMES);
        return true;
      }

      lfh_deinit();
      vars_set_num_frames(arg_16t_buf);
      lfh_init_file();
      lfh_init();

      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, NUM_FRAMES_SET);
      return true;

    case SET_BRIGHTNESS:
      // <opcode><brightness uint8_t>
      if (len != 2)
      {
        wsockh_send_resp(client, ERR_ARGS_MISMATCH);
        return true;
      }

      vars_set_brightness(data[1]);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, BRIGHTNESS_SET);
      return true;

    case SET_NUM_PIXELS:
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;

      lfh_deinit();
      vars_set_num_pixels(arg_16t_buf);
      lfh_init_file();
      lfh_init();

      wsockh_send_resp(client, SUCCESS_NO_DATA);
      evh_fire_event(&client_id, NUM_PIXELS_SET);
      return true;

    case SET_EVT_SUB:
      // <opcode><eventcode uint8_t><state uint8_t>
      if (len != 3)
      {
        wsockh_send_resp(client, ERR_ARGS_MISMATCH);
        return true;
      }

      // Create registration if not yet exists
      if (evh_exists_client(client->id()))
      {
        // Cannot add any other listeners!
        if (!evh_add_client(client->id()))
        {
          wsockh_send_resp(client, ERR_EVT_SUBS_BUF_FULL);
          return true;
        }
      }
      
      // Check if event code is out of range
      if (data[1] >= CEV_NUM_EVENTS)
      {
        wsockh_send_resp(client, ERR_UNKNOWN_EVT_REQ);
        return true;
      }

      // Set subscription status
      evh_set_subscription(client->id(), (CommEventCode) data[1], data[2] != 0);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      return true;

    case GET_FRAME_DUR:
      wsockh_send_arg_numeric(client, vars_get_frame_dur(), 2);
      return true;

    case GET_NUM_FRAMES:
      wsockh_send_arg_numeric(client, vars_get_num_frames(), 2);
      return true;

    case GET_FRAME_SLOTS:
      wsockh_send_arg_numeric(client, lfh_get_frame_slots(), 2);
      return true;

    case GET_BRIGHTNESS:
      wsockh_send_arg_numeric(client, vars_get_brightness(), 2);
      return true;

    case GET_WIFI_SSID:
      str_arg_buf = vars_get_wifi_ssid();
      wsockh_send_strings(client, &str_arg_buf, 1);
      return true;

    case GET_SD_SIZE:
      wsockh_send_arg_numeric(client, sdh_get_total_size_mb(), 4);
      return true;

    case GET_DEV_NAME:
      str_arg_buf = vars_get_dev_name();
      wsockh_send_strings(client, &str_arg_buf, 1);
      return true;

    case GET_NUM_PIXELS:
      wsockh_send_arg_numeric(client, vars_get_num_pixels(), 2);
      return true;

    case REBOOT:
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      dbg_log("Restarting device in %dms!\n", WSOCKH_REB_DEL);
      rbh_request_schedule(WSOCKH_REB_DEL );
      return true;

    // Not handleable with one request-frame
    default:
      return false;
  }
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
                            Event notification                             
============================================================================
*/

/**
 * @brief Notify a client of an occurred event
 * 
 * @param client_id ID of client
 * @param event Event that fired
 */
void wsockh_notify_client(uint32_t client_id, CommEventCode event)
{
  AsyncWebSocketClient *cl = wsockh_ws.client(client_id);
  const char* str_arg = 0;

  // Client cannot be found, cancel
  if (cl == NULL)
  {
    // Remove invalid entry
    evh_remove_client(client_id);
    return;
  }

  // Local event variable to have a pointer for function calls
  uint8_t ev = event;

  switch (event)
  {
    case FRAME_DUR_SET:
      wsockh_send_arg_numeric(cl, vars_get_frame_dur(), 2, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case NUM_FRAMES_SET:
      wsockh_send_arg_numeric(cl, vars_get_num_frames(), 2, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case BRIGHTNESS_SET:
      wsockh_send_arg_numeric(cl, vars_get_brightness(), 1, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case WIFI_CRED_SET:
      str_arg = vars_get_wifi_ssid();
      wsockh_send_strings(cl, &str_arg, 1, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case SD_CARD_STATE:
      wsockh_send_arg_numeric(cl, sdh_io_available() ? 0x1 : 0x0, 1, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case DEV_NAME_SET:
      str_arg = vars_get_dev_name();
      wsockh_send_strings(cl, &str_arg, 1, SUBSCRIBED_EV_FIRED, &ev);
      break;

    case NUM_PIXELS_SET:
      wsockh_send_arg_numeric(cl, vars_get_num_pixels(), 2, SUBSCRIBED_EV_FIRED, &ev);
      break;

    default:
      dbg_log("Could not set yet unknown event %d!\n", ev);
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
  evh_set_notifier(wsockh_notify_client);
  wsockh_ws.onEvent(wsockh_ev_handler);
  wsrvh_register_handler(&wsockh_ws);
}

void wsockh_cleanup()
{
  wsockh_ws.cleanupClients();
}
