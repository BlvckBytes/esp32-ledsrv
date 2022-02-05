#ifndef web_socket_handler_h
#define web_socket_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <ESPAsyncWebServer.h>
#include <wsockh/web_socket_handler.h>
#include <led_frame_handler.h>
#include <web_server_handler.h>
#include <comm/comm_opcode.h>
#include <comm/comm_resultcode.h>
#include <dbg_log.h>
#include <sd_handler.h>
#include <variable_store.h>
#include <reboot_handler.h>
#include <event_handler.h>
#include <timeout_handler.h>

#include <wsockh/wsockh_utils.h>
#include <wsockh/wsockh_notifier.h>
#include <wsockh/wsockh_remainder.h>

// Request responders
#include <wsockh/responders/wsockh_set_frame_dur.h>
#include <wsockh/responders/wsockh_set_num_frames_num_pixels.h>
#include <wsockh/responders/wsockh_set_brightness.h>
#include <wsockh/responders/wsockh_set_evt_sub.h>
#include <wsockh/responders/wsockh_set_frame_cont.h>
#include <wsockh/responders/wsockh_set_wifi_cred.h>
#include <wsockh/responders/wsockh_set_dev_name.h>
#include <wsockh/responders/wsockh_get_frame_dur.h>
#include <wsockh/responders/wsockh_get_num_frames_num_pixels.h>
#include <wsockh/responders/wsockh_get_frame_slots.h>
#include <wsockh/responders/wsockh_get_frame_cont.h>
#include <wsockh/responders/wsockh_get_brightness.h>
#include <wsockh/responders/wsockh_get_wifi_ssid.h>
#include <wsockh/responders/wsockh_get_sd_size.h>
#include <wsockh/responders/wsockh_get_dev_name.h>
#include <wsockh/responders/wsockh_cmd_reboot.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// Websocket config
#define WSOCKH_PATH "/ws"
#define WSOCKH_MSGBUF_SIZE 4096

/*
============================================================================
                                   Types                                    
============================================================================
*/

/**
 * @brief Used to represend individual request handler functions
 */
typedef void (*wsockh_req_handler_t)(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
);

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