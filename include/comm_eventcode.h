#ifndef comm_eventcode_h
#define comm_eventcode_h

// INFO: This counts the number of available events
#define CEV_NUM_EVENTS 8

// WARNING: Do NOT offset these, as they are used as array indices
typedef enum {
  FRAME_DUR_SET,
  NUM_FRAMES_NUM_PIXELS_SET,
  FRAME_CONT_SET,
  BRIGHTNESS_SET,
  WIFI_CRED_SET,
  SD_CARD_STATE,
  DEV_NAME_SET,
} CommEventCode;

#endif