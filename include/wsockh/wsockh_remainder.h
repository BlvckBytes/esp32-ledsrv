#ifndef wsockh_remainder_h
#define wsockh_remainder_h

#include <inttypes.h>
#include <AsyncWebSocket.h>
#include <dbg_log.h>

#define WSOCKH_REQUEST_REMAINDERS_LEN 8

/**
 * @brief Used to save the remaining bytes of a early terminated request
 */
typedef struct
{
  AsyncWebSocketClient *client;
  uint64_t remainder;
} wsockh_req_remainder_t;

/**
 * @brief Check if a request is terminated already and thus can be ignored
 * 
 * @param client Origin of the target request
 * @param read_len Length of current packet
 * 
 * @return true Request can be ignored
 * @return false Request is valid and needs processing
 */
bool wsockh_is_request_terminated(AsyncWebSocketClient *client, size_t read_len);

/**
 * @brief Terminate a request by marking the remaining bytes that are to be received as ignoreable
 * 
 * @param client Origin of the target request
 * @param info Frame info to calculate remaining size
 * @param read_len Length of current packet
 */
void wsockh_terminate_request(AsyncWebSocketClient *client, AwsFrameInfo *info, size_t read_len);

#endif