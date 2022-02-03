#ifndef timeout_handler_h
#define timeout_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <inttypes.h>
#include <Arduino.h>
#include <dbg_log.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

#define TOH_REGISTRY_SLOTS 8

/*
============================================================================
                                   Types                                    
============================================================================
*/

typedef void (*callback)();

typedef struct
{
  uint32_t duration;
  uint32_t stamp;
  callback cb;
} TimeoutEntry;

/*
============================================================================
                                   Handler                                  
============================================================================
*/

/**
 * @brief Create a new timeout for a specific duration
 * 
 * @param duration_ms Duration in milliseconds
 * @param cb Callback function on completion
 * @return int Timeout handle
 */
int toh_create_timeout(uint32_t duration_ms, callback cb);

/**
 * @brief Reset a timeout's duration
 * 
 * @param handle Timeout handle
 */
void toh_reset(int handle);

/**
 * @brief Check registry for due timeouts
 */
void toh_check();

/**
 * @brief Checks whether or not this handle is still active
 * 
 * @param handle Handle to check for
 * @return true Handle is active
 * @return false Handle is non-existing or has been processed
 */
bool toh_is_active(int handle);

#endif