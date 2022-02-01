#ifndef comm_opcode_h
#define comm_opcode_h

typedef enum {
  /*         NAME               |  VALUE  | SINGLE FRAME | R/W | ARGS                 */

  SET_FRAME_DUR                 = 0x00, //      y          W    <uint_16t>
  SET_NUM_FRAMES_NUM_PIXELS     = 0x01, //      y          W    <uint_16t><uint16_t>
  SET_FRAME_CONT                = 0x02, //      n          W    <uint_16t><FRAME>
  SET_BRIGHTNESS                = 0x03, //      y          W    <uint_8t>
  SET_WIFI_CRED                 = 0x04, //      n          W    <ssid\0><pass\0>
  SET_EVT_SUB                   = 0x05, //      y          W    <uint8_t><uint8_t>
  SET_DEV_NAME                  = 0x06, //      n          W    <dev_name\0>
  SET_NUM_PIXELS                = 0x07, //      y          W    <uint16_t>

  GET_FRAME_DUR                 = 0x80, //      y          R    /
  GET_NUM_FRAMES_NUM_PIXELS     = 0x81, //      y          R    /
  GET_FRAME_SLOTS               = 0x82, //      y          R    /
  GET_FRAME_CONT                = 0x83, //      n          R    <uint_16t>
  GET_BRIGHTNESS                = 0x84, //      y          R    /
  GET_WIFI_SSID                 = 0x85, //      y          R    /
  GET_SD_SIZE                   = 0x86, //      y          R    /
  GET_DEV_NAME                  = 0x87, //      y          R    /

  REBOOT                        = 0xFF, //      y          /    /
} CommOpCode;

#endif