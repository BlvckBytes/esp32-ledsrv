#ifndef wsockh_set_frame_cont_h
#define wsockh_set_frame_cont_h

#include <AsyncWebSocket.h>
#include <wsockh/wsockh_utils.h>
#include <inttypes.h>
#include <dbg_log.h>
#include <timeout_handler.h>
#include <led_frame_handler.h>

// Timeout until drawing is resumed after a frame-set
// WARNING: Don't choose a too-low value to avoid race-conditions
#define WSOCKH_SET_FRAME_TIMEOUT 2000

typedef struct
{
  AsyncWebSocketClient *client;
  uint16_t frame_index;
  uint8_t *content;
} wsockh_frame_cont_set_t;

/**
 * @brief Allocates a frame content set struct on the heap
 * 
 * @param client Request issuer
 * @param frame_index Index of the frame
 * @param content Content of the frame
 * @param content_len Length of content
 * @return wsockh_frame_cont_set_t* 
 */
wsockh_frame_cont_set_t *wsockh_alloc_wsockh_frame_cont_set(
  AsyncWebSocketClient *client,
  uint16_t frame_index,
  uint8_t *content,
  uint16_t content_len
);

/**
 * @brief Deallocate a frame content set structure
 * 
 * @param data Data to free up
 */
void wsockh_dealloc_wsockh_frame_cont_set(wsockh_frame_cont_set_t *data);

typedef void (*wsockh_req_handler_t)(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

void wsockh_set_frame_cont(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif