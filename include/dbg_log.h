#ifndef dbg_log_h
#define dbg_log_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <Arduino.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// Whether or not to route debug log calls
#define DBGL_ACTIVE false
#define DBGL_BAUD 115200

// Debug is active, route requests to serial comm
#if DBGL_ACTIVE
#define dbg_log(...) Serial.printf(__VA_ARGS__)

// Debug is inactive, do nothing
#else
#define dbg_log(...) (void) 0
#endif

/*
============================================================================
                            Basic debug control                             
============================================================================
*/

/**
 * @brief Initialize the debug logging functionality
 */
void init_dbg_log();

#endif