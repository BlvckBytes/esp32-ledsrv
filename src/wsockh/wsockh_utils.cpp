#include <wsockh/wsockh_utils.h>

void wsockh_send_resp(
  AsyncWebSocketClient *client,
  CommResultCode code,
  uint8_t *bytes,
  int num_bytes
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

void wsockh_send_arg_numeric(
  AsyncWebSocketClient *client,
  uint64_t value,
  uint8_t num_bytes,
  CommResultCode result_code,
  uint8_t *prefix
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

void wsockh_send_strings(
  AsyncWebSocketClient *client,
  const char **strings,
  size_t num_strings,
  CommResultCode result_code,
  uint8_t *prefix
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