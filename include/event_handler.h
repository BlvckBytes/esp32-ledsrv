#ifndef event_handler_h
#define event_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <inttypes.h>
#include <comm/comm_eventcode.h>
#include <dbg_log.h>
#include <AsyncWebSocket.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

#define EVH_CLIENTBUF_SIZE 16

/*
============================================================================
                                   Types                                    
============================================================================
*/

typedef struct {
  bool active = false;
  uint32_t client_id = 0;
  int subscriptions[CEV_NUM_EVENTS] = { 0 };
} EventHandlerRegistration;

typedef void (*evh_notifier_t)(AsyncWebSocket*, uint32_t, CommEventCode);

/*
============================================================================
                             Client Add/Remove                              
============================================================================
*/

/**
 * @brief Add a new client as a consumer
 * 
 * @param client_id ID of the client
 * 
 * @return true Client registered
 * @return false Unsuccessful, no more space for clients
 */
bool evh_add_client(uint32_t client_id);

/**
 * @brief Remove an existing client by their ID
 * 
 * @param client_id ID of the client
 * 
 * @return true Client unregistered
 * @return false Client wasn't registered
 */
bool evh_remove_client(uint32_t client_id);

/**
 * @brief Checks whether or not a client registration exists
 * 
 * @param client_id ID of the client
 * @return true Client is registered
 * @return false Client is unknown
 */
bool evh_exists_client(uint32_t client_id);

/*
============================================================================
                               Notifications                                
============================================================================
*/

/**
 * @brief Set the notifier function that'll take care of actually
 * sending events to clients
 * 
 * @param sock Socket connection reference
 * @param notifier Notifier callback function
 */
void evh_set_notifier(AsyncWebSocket *sock, evh_notifier_t notifier);

/*
============================================================================
                                   Events                                   
============================================================================
*/

/**
 * @brief Fire an event to be broadcasted
 * 
 * @param cause_client_id ID of the causing client, -1 if no client was involved
 * @param event Event to fire
 * 
 * @return true Event fired
 * @return false No notifier set beforehand
 */
bool evh_fire_event(long cause_client_id, CommEventCode event);

/*
============================================================================
                               Subscriptions                                
============================================================================
*/

/**
 * @brief Set the subscription state for a specific event
 * 
 * @param client_id ID of the client
 * @param event Event in focus
 * @param state Subscription status
 * 
 * @return true State set successfully
 * @return false Client not registered
 */
bool evh_set_subscription(uint32_t client_id, CommEventCode event, bool state);

#endif