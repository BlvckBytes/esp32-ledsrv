#ifndef lfh_pause_h
#define lfh_pause_h

#include <stddef.h>
#include <inttypes.h>
#include <timeout_handler.h>

#define LFH_PAUSE_CB_RINGBUF_SIZE 8
#define LFH_PAUSE_TIMEOUT_MS 3000

typedef void (*lfh_pause_req_t)(void *arg);
typedef void (*lfh_resume_t)();

typedef struct {
  lfh_pause_req_t cb;
  void *arg;
} PauseRequest;

/**
 * @brief Register a pause request
 * 
 * @param avail Pause available callback
 */
void lfh_pause_register(lfh_pause_req_t avail, void *arg);

/**
 * @brief Check whether or not the LFH is currently paused
 */
bool lfh_is_paused();

/**
 * @brief Check for active pause requests
 * 
 * @return true Active pause requests available
 * @return false No active pause requests
 */
bool lfh_pause_check();

/**
 * @brief Call all available pause requests
 * 
 * @param timeout Pause request timeout callback to resume functionallity
 */
void lfh_pause_call(lfh_resume_t timeout);

#endif