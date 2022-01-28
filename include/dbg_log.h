#ifndef dbg_log_h
#define dbg_log_h

#include <Arduino.h>

// Whether or not to route debug log calls
#define DEBUG_MODE_ACTIVE true

// Debug is active, route requests to serial comm
#if DEBUG_MODE_ACTIVE
#define dbg_log(...) Serial.printf(__VA_ARGS__)

// Debug is inactive, do nothing
#else
#define dbg_log(...) (void) 0
#endif

/**
 * @brief Initialize the debug logging functionality
 * 
 * @param baud Serial communication speed
 */
void init_dbg_log(int baud);

#endif