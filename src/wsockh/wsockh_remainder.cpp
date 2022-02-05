#include <wsockh/wsockh_remainder.h>

// Request remainder buffer
static wsockh_req_remainder_t wsockh_request_remainders[WSOCKH_REQUEST_REMAINDERS_LEN];
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
    wsockh_req_remainder_t *tar = &wsockh_request_remainders[i];

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
  wsockh_req_remainder_t rem = { client, remainder };

  // Add to ringbuffer
  wsockh_request_remainders[wsockh_request_remainder_pointer] = rem;
  dbg_log("Writing request ignore at %" PRIu8 " for client %" PRIu32 " remaining %" PRIu64 "!\n", wsockh_request_remainder_pointer, client->id(), remainder);
  wsockh_request_remainder_pointer = (wsockh_request_remainder_pointer + 1) % WSOCKH_REQUEST_REMAINDERS_LEN;
}
