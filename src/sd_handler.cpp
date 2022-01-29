#include <sd_handler.h>

// All variables are stored as JSON here
File sdh_file_vars;

// All frames are stored as BINARY here
File sdh_file_frames;

void sdh_init()
{
  // Begin SD library using known pin layout
  if (SD.begin(SDH_PIN_CS)) {
    dbg_log("SD card slot initialized (type=%" PRIu8 ")!\n", SD.cardType());
  } else
    dbg_log("Could not initialize SD card slot!\n");
}