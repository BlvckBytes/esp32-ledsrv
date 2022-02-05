#ifndef wsockh_notifier_h
#define wsockh_notifier_h

#include <inttypes.h>
#include <comm/comm_eventcode.h>
#include <wsockh/wsockh_utils.h>
#include <dbg_log.h>

/**
 * @brief Notify a client of an occurred event
 * 
 * @param socket Socket server reference
 * @param client_id ID of client
 * @param event Event that fired
 */
void wsockh_notify_client(AsyncWebSocket *socket, uint32_t client_id, CommEventCode event);

#endif