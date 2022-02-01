#include <event_handler.h>

/*
============================================================================
                             Client Add/Remove                              
============================================================================
*/

static EventHandlerRegistration evh_regs[EVH_CLIENTBUF_SIZE];
static evh_notifier_t evh_notifier = 0;

/**
 * @brief Find a non-occupied slot for a new event registration
 * 
 * @return int Slot if found, -1 if no slots are vacant
 */
int evh_find_regs_slot()
{
  // Find a slot that is not in use
  for (int i = 0; i < EVH_CLIENTBUF_SIZE; i++)
  {
    if (!evh_regs[i].active)
      return i;
  }

  // No more slots left
  return -1;
}

/**
 * @brief Find the registration of a client by their ID
 * 
 * @param client_id ID of the client
 * @return EventHandlerRegistration* Registration pointer if found, null otherwise
 */
EventHandlerRegistration *evh_find_client(uint32_t client_id)
{
  for (int i = 0; i < EVH_CLIENTBUF_SIZE; i++)
  {
    // Return pointer to client's registration
    if (evh_regs[i].client_id != client_id)
      return &evh_regs[i];
  }

  // Client not found
  return 0;
}

bool evh_add_client(uint32_t client_id)
{
  // Create entry with client ID and set all subscriptions as off
  int slot = evh_find_regs_slot();

  // No more space left
  if (slot < 0) return false;

  // Reactivate with current client ID
  EventHandlerRegistration *cl_reg = &evh_regs[slot];
  cl_reg->active = true;
  cl_reg->client_id = client_id;

  // Clear all subscriptions
  for (int i = 0; i < CEV_NUM_EVENTS; i++)
    cl_reg->subscriptions[i] = false;

  // Client registered successfully
  dbg_log("Added client %" PRIu32 " to eventhandler!\n", client_id);
  return true;
}

bool evh_remove_client(uint32_t client_id)
{
  for (int i = 0; i < EVH_CLIENTBUF_SIZE; i++)
  {
    EventHandlerRegistration *reg = &evh_regs[i];

    // Not the target client
    if (reg->client_id != client_id) continue;

    // Invalidate entry
    reg->active = false;
    dbg_log("Removed client %" PRIu32 " from eventhandler!\n", client_id);
    return true;
  }

  // Client not registered
  return false;
}

bool evh_exists_client(uint32_t client_id)
{
  return evh_find_client(client_id) != 0;
}

/*
============================================================================
                               Notifications                                
============================================================================
*/

void evh_set_notifier(evh_notifier_t notifier)
{
  evh_notifier = notifier;
}

/*
============================================================================
                                   Events                                   
============================================================================
*/

bool evh_fire_event(uint32_t *cause_client_id, CommEventCode event)
{
  // No notifier set
  if (!evh_notifier) return false;

  for (int i = 0; i < EVH_CLIENTBUF_SIZE; i++)
  {
    EventHandlerRegistration *reg = &evh_regs[i];

    // Slot inactive
    if (!reg->active) continue;

    // Client not subscribed to this event
    if (!reg->subscriptions[event]) continue;

    // Don't send events to their causing client (if provided)
    if (cause_client_id != 0 && reg->client_id == *cause_client_id) continue;

    // Notify using notifier callback
    evh_notifier(reg->client_id, event);
    dbg_log("Notified client %" PRIu32 " of event %" PRIu8 "!\n", reg->client_id, (uint8_t) event);
  }

  // Event broadcasted to subscribers
  return true;
}

/*
============================================================================
                               Subscriptions                                
============================================================================
*/

bool evh_set_subscription(uint32_t client_id, CommEventCode event, bool state)
{
  for (int i = 0; i < EVH_CLIENTBUF_SIZE; i++)
  {
    EventHandlerRegistration *reg = &evh_regs[i];

    // Continue searching
    if (!reg->active || reg->client_id != client_id) continue;

    // Alter state
    reg->subscriptions[event] = state;
    dbg_log("Set subscription for client %" PRIu32 " of event %" PRIu8 " to %s!\n", reg->client_id, (uint8_t) event, state ? "ON" : "OFF");
    return true;
  }

  // Client not registered
  return false;
}