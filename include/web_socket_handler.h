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


/**
 * @brief Initialize the websocket-server
 */
void wsockh_init();

#endif