#ifndef wsockh_get_frame_cont_h
#define wsockh_get_frame_cont_h

#include <AsyncWebSocket.h>
#include <inttypes.h>
#include <wsockh/wsockh_utils.h>
#include <led_frame_handler.h>
#include <timeout_handler.h>

// Timeout until drawing is resumed after a frame-get
// WARNING: Don't choose a too-low value to avoid race-conditions
#define WSOCKH_GET_FRAME_TIMEOUT 2000

/**
 * @brief Represents the request of a client targetting a specific frame,
 * used as a callback object
 */
typedef struct
{
  AsyncWebSocketClient *client;
  uint16_t frame_index;
} wsockh_frame_cont_req_t;

/**
 * @brief Allocates a frame content request struct on the heap
 * 
 * @param client Request issuer
 * @param frame_index Index of target frame
 */
wsockh_frame_cont_req_t *wsockh_alloc_frame_cont_req(AsyncWebSocketClient *client, uint16_t frame_index);

/**
 * @brief Deallocate a frame content request structure
 * 
 * @param data Data to free up
 */
void wsockh_dealloc_frame_cont_req(wsockh_frame_cont_req_t *data);

void wsockh_get_frame_cont(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

#endif