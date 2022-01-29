#include <web_socket_handler.h>
#include <led_frame_handler.h>

AsyncWebSocket wsockh_ws(WSOCKH_PATH);

// Request remainder buffer
static AsyncWebSocketRequestRemainder wsockh_request_remainders[WSOCKH_REQUEST_REMAINDERS_LEN];
static uint8_t wsockh_request_remainder_pointer = 0;

/**
 * @brief Send a binary response to the client while adding a leading response code
 * 
 * @param client Recipient of the message
 * @param code Result code to append as head
 * @param args Argument bytes
 * @param num_args Number of arguments
 */
void wsockh_send_resp(
  AsyncWebSocketClient *client,
  CommResultCode code,
  uint8_t *args = NULL,
  int num_args = 0
)
{
  // Create buffer with first element being the code
  uint8_t buf[num_args + 1] = {};
  buf[0] = code;

  // Copy over args into buffer
  if (num_args != 0)
    memcpy(&buf[1], args, num_args);

  // Send opcode with args
  client->binary(buf, num_args + 1);
}

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
 * @brief Send a 16 bit argument as the response to the client
 * 
 * @param client Recipient of message
 * @param value 16 bit value to send
 */
void wsockh_send_arg_16t(AsyncWebSocketClient *client, uint16_t value)
{
  uint8_t data_buf[2] = { 0 };

  // msB
  data_buf[0] = (value >> 8) & 0xFF;

  // lsB
  data_buf[1] = value & 0xFF;

  // Send buffer to client
  wsockh_send_resp(client, SUCCESS_DATA_FOLLOWS, data_buf, 2);
}

/**
 * @brief Try to handle opcodes that are single-packeted (small data)
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

  switch (data[0])
  {
    case SET_FRAME_DUR:
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;
      lfh_set_frame_duration(arg_16t_buf);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      return true;

    case SET_NUM_FRAMES:
      if (!wsockh_read_arg_16t(client, 0, data, len, &arg_16t_buf)) return true;
      lfh_set_num_frames(arg_16t_buf);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      return true;

    case SET_BRIGHTNESS:
      if (len <= 1)
      {
        wsockh_send_resp(client, ERR_ARGS_MISMATCH);
        return true;
      }

      lfh_set_brightness(data[1]);
      wsockh_send_resp(client, SUCCESS_NO_DATA);
      return true;

    case GET_FRAME_DUR:
      wsockh_send_arg_16t(client, lfh_get_frame_duration());
      return true;

    case GET_NUM_FRAMES:
      wsockh_send_arg_16t(client, lfh_get_num_frames());
      return true;

    case GET_FRAME_SLOTS:
      wsockh_send_arg_16t(client, lfh_get_frame_slots());
      return true;

    case GET_BRIGHTNESS:
      wsockh_send_arg_16t(client, lfh_get_brightness());
      return true;

    // Not handleable in one frame
    default:
      return false;
  }
}

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
// WARNING: This is quite complicated and needs extensive testing!
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

  // OpCode always is represented by the first byte
  uint8_t opcode = data[0];

  // Handle single frame requests
  if (wsockh_handle_single_packet_req(client, info, data, len)) {
    dbg_log("Single frame request opcode=%#04X handled!\n", opcode);

    // Don't process this request any further, if it has any trailing data
    wsockh_terminate_request(client, info, len);
    return;
  }

  dbg_log("Non single packet request encountered, not yet implemented!\n");
}

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

void wsockh_init()
{
  wsockh_ws.onEvent(wsockh_ev_handler);
  wsrvh_register_handler(&wsockh_ws);
}

void wsockh_cleanup()
{
  wsockh_ws.cleanupClients();
}