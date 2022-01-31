#ifndef reboot_handler_h
#define reboot_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <Arduino.h>
#include <inttypes.h>

/*
============================================================================
                                  Scheduler                                 
============================================================================
*/

/**
 * @brief Request a reboot schedule
 * 
 * @param ms_delay Delay until reboot from time of invocation
 */
void rbh_request_schedule(uint32_t ms_delay);

/**
 * @brief Check if a reboot request is to be executed
 */
void rbh_check_requests();

#endif