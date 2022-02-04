#include <wsockh/responders/wsockh_set_evt_sub.h>

void wsockh_set_evt_sub(
  AsyncWebSocketClient *client,
  uint8_t *data,
  size_t len
)
{
  // <opcode><eventcode uint8_t><state uint8_t>
  if (len != 3)
  {
    wsockh_send_resp(client, ERR_ARGS_MISMATCH);
    return;
  }

  // Create registration if not yet exists
  if (evh_exists_client(client->id()))
  {
    // Cannot add any other listeners!
    if (!evh_add_client(client->id()))
    {
      wsockh_send_resp(client, ERR_EVT_SUBS_BUF_FULL);
      return;
    }
  }
  
  // Check if event code is out of range
  if (data[1] >= CEV_NUM_EVENTS)
  {
    wsockh_send_resp(client, ERR_UNKNOWN_EVT_REQ);
    return;
  }

  // Set subscription status
  evh_set_subscription(client->id(), (CommEventCode) data[1], data[2] != 0);
  wsockh_send_resp(client, SUCCESS_NO_DATA);
  return;
}