#ifndef web_socket_handler_h
#define web_socket_handler_h

#include <ESPAsyncWebServer.h>
#include <web_socket_handler.h>
#include <web_server_handler.h>
#include <comm_opcode.h>
#include <comm_resultcode.h>
#include <dbg_log.h>

// Websocket config
#define WSOCKH_PATH "/ws"
#define WSOCKH_REQUEST_REMAINDERS_LEN 8

/**
 * @brief Used to save the remaining bytes of a early terminated request
 */
typedef struct AsyncWebSocketRequestRemainder
{
  AsyncWebSocketClient *client;
  uint64_t remainder;
} AsyncWebSocketRequestRemainder;

/**
 * @brief Initialize the websocket-server
 */
void wsockh_init();

/**
 * @brief Clean up resources no longer in use
 */
void wsockh_cleanup();

#endif