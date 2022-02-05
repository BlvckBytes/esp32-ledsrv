#ifndef wsockh_utils_h
#define wsockh_utils_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <comm/comm_resultcode.h>
#include <variable_store.h>

#define WSOCKH_STRARGBUF_SIZE 4

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
);

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
);

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
);

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
);

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
);

#endif