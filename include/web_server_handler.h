#ifndef web_server_handler_h
#define web_server_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <ESPAsyncWebServer.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// Web server config
#define WSRVH_PORT 80
#define WSRVH_ROOT_WELCOME "<h1>You're on an ESP32-LEDSRV</h1>"

/*
============================================================================
                            Basic server control                            
============================================================================
*/

/**
 * @brief Initialize the web-server
 */
void wsrvh_init();

/*
============================================================================
                             Extension handling                             
============================================================================
*/

/**
 * @brief Register a handler that serves a subset of requests by a filter
 * 
 * @param handler Handler to register
 */
void wsrvh_register_handler(AsyncWebHandler *handler);

#endif