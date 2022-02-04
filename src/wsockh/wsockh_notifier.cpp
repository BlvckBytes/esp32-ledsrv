#include <wsockh/wsockh_notifier.h>

void wsockh_notify_client(
  AsyncWebSocket *socket,
  uint32_t client_id,
  CommEventCode event
)
{
  AsyncWebSocketClient *cl = socket->client(client_id);
  const char* str_arg = 0;

  // Client cannot be found, cancel
  if (cl == NULL)
  {
    // Remove invalid entry
    evh_remove_client(client_id);
    return;
  }

  // Local event variable to have a pointer for function calls
  uint8_t ev = event;

  if (ev == FRAME_DUR_SET)
  {
    wsockh_send_arg_numeric(cl, vars_get_frame_dur(), 2, SUBSCRIBED_EV_FIRED, &ev);
    return;
  }

  if (ev == NUM_FRAMES_NUM_PIXELS_SET)
  {
    wsockh_send_arg_numeric(cl,
      ((vars_get_num_frames() << 16) | vars_get_num_pixels()),
      4,
      SUBSCRIBED_EV_FIRED, &ev
    );
    return;
  }

  if (ev == BRIGHTNESS_SET)
  {
    wsockh_send_arg_numeric(cl, vars_get_brightness(), 1, SUBSCRIBED_EV_FIRED, &ev);
    return;
  }

  if (ev == WIFI_CRED_SET)
  {
    str_arg = vars_get_wifi_ssid();
    wsockh_send_strings(cl, &str_arg, 1, SUBSCRIBED_EV_FIRED, &ev);
    return;
  }

  if (ev == SD_CARD_STATE)
  {
    wsockh_send_arg_numeric(cl, sdh_io_available() ? 0x1 : 0x0, 1, SUBSCRIBED_EV_FIRED, &ev);
    return;
  }

  if (ev == DEV_NAME_SET)
  {
    str_arg = vars_get_dev_name();
    wsockh_send_strings(cl, &str_arg, 1, SUBSCRIBED_EV_FIRED, &ev);
    return;
  }

  // Unknown event encountered
  dbg_log("Could not set yet unknown event %d!\n", ev);
}