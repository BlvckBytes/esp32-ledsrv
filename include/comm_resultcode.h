#ifndef comm_resultcode_h
#define comm_resultcode_h

typedef enum {
  /*         NAME               |  VALUE  | */
  SUBSCRIBED_EV_FIRED             = 0xFE,
  SUCCESS_NO_DATA                 = 0xFF,
  SUCCESS_DATA_FOLLOWS            = 0x00,

  // Error
  ERR_TEXT_RECV                   = 0x01,
  ERR_ARGS_MISMATCH               = 0x02,
  ERR_NUM_PIXEL_MISMATCH          = 0x03,
  ERR_INVAL_FRAME_IND             = 0x04,
  ERR_INVAL_PIXEL_CLR             = 0x05,
  ERR_INVAL_BRIGHTNESS            = 0x06,
  ERR_STRING_UNTERMINATED         = 0x07,
  ERR_REQ_EMPTY                   = 0x08,
  ERR_NO_SD_ACC                   = 0x09,
  ERR_FRAG_REQ                    = 0x0A,
  ERR_REQ_TOO_LONG                = 0x0B,
  ERR_UNKNOWN_OPCODE              = 0x0C,
  ERR_STR_TOO_LONG                = 0x0D,
  ERR_TOO_MANY_STRARGS            = 0x0E,
  ERR_EVT_SUBS_BUF_FULL           = 0x0F,
  ERR_UNKNOWN_EVT_REQ             = 0x10,
  ERR_INVAL_DEV_NAME              = 0x11,
  ERR_TOO_MANY_FRAMES             = 0x12,
  ERR_TOO_MANY_PIXELS             = 0x13
} CommResultCode;

#endif