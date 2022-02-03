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
typedef struct AsyncWebSocketRequestRemainder
{
  AsyncWebSocketClient *client;
  uint64_t remainder;
} AsyncWebSocketRequestRemainder;

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