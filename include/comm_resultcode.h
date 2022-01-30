#ifndef comm_resultcode_h
#define comm_resultcode_h

typedef enum {
  // Success
  SUCCESS_NO_DATA = 0xFF,
  SUCCESS_DATA_FOLLOWS = 0x0,

  // Error
  ERR_TEXT_RECV = 0x1,
  ERR_ARGS_MISMATCH = 0x2,
  ERR_NUM_PIXEL_MISMATCH = 0x3,
  ERR_INVAL_FRAME_IND = 0x4,
  ERR_INVAL_PIXEL_CLR = 0x5,
  ERR_INVAL_BRIGHTNESS = 0x6,
  ERR_STRING_UNTERMINATED = 0x7,
  ERR_REQ_EMPTY = 0x8,
  ERR_NO_SD_ACC = 0x9
} CommResultCode;

#endif