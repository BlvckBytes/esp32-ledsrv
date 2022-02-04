#ifndef web_socket_handler_h
#define web_socket_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <ESPAsyncWebServer.h>
#include <web_socket_handler.h>
#include <led_frame_handler.h>
#include <web_server_handler.h>
#include <comm_opcode.h>
#include <comm_resultcode.h>
#include <dbg_log.h>
#include <sd_handler.h>
#include <variable_store.h>
#include <reboot_handler.h>
#include <event_handler.h>
#include <timeout_handler.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// Websocket config
#define WSOCKH_PATH "/ws"
#define WSOCKH_REQUEST_REMAINDERS_LEN 8
#define WSOCKH_MSGBUF_SIZE 4096
#define WSOCKH_STRARGBUF_SIZE 8

// Reboot opcode
#define WSOCKH_REB_DEL 5000

// Timeout until drawing is resumed after a frame-set
// WARNING: Don't choose a too-low value to avoid race-conditions
#define WSOCKH_SET_FRAME_TIMEOUT 2000

/*
============================================================================
                                   Types                                    
============================================================================
*/

/**
 * @brief Used to save the remaining bytes of a early terminated request
 */
typedef struct
{
  AsyncWebSocketClient *client;
  uint64_t remainder;
} wsockh_req_remainder_t;

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

/*
============================================================================
                            Basic socket control                            
============================================================================
*/

/**
 * @brief Initialize the websocket-server
 */
void wsockh_init();

/**
 * @brief Clean up resources no longer in use
 */
void wsockh_cleanup();

#endif