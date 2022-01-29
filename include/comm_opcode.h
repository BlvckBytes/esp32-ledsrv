#ifndef comm_opcode_h
#define comm_opcode_h

typedef enum {
  /* |        NAME           | SINGLE FRAME | R/W | ARGS */
  SET_FRAME_DUR = 0x0,     //       y          W    <uint_16t>
  SET_NUM_FRAMES = 0x1,    //       y          W    <uint_16t>
  SET_FRAME_CONT = 0x2,    //       n          W    <uint_16t><FRAME>
  SET_BRIGHTNESS = 0x3,    //       y          W    <uint_8t>
  SET_WIFI_CRED = 0x4,     //       ?          W    <ssid\0><pass\0>

  GET_FRAME_DUR = 0x80,    //       y          R    /
  GET_NUM_FRAMES = 0x81,   //       y          R    /
  GET_FRAME_SLOTS = 0x82,  //       y          R    /
  GET_FRAME_CONT = 0x83,   //       n          R    <uint_16t>
  GET_BRIGHTNESS = 0x84,   //       y          R    /
  GET_WIFI_SSID = 0x85     //       y          R    /
} CommOpCode;

#endif