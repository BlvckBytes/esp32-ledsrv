#ifndef comm_eventcode_h
#define comm_eventcode_h

// INFO: This counts the number of available events
#define CEV_NUM_EVENTS 8

// WARNING: Do NOT offset these, as they are used as array indices
typedef enum {
  /*         NAME               |  VALUE  |    ARGS                 */

  FRAME_DUR_SET                   = 0x0, //   <frame_dur uint16_t>
  NUM_FRAMES_NUM_PIXELS_SET,             //   <num_frames uint16_t><num_pixels uint16_t>
  FRAME_CONT_SET,                        //   <frame_index uint16_t><FRAME>
  BRIGHTNESS_SET,                        //   <brightness uint8_t>
  WIFI_CRED_SET,                         //   <ssid_bytes\0>
  SD_CARD_STATE,                         //   <state_bool uint8_t>
  DEV_NAME_SET,                          //   <dev_name_bytes\0>
} CommEventCode;

#endif