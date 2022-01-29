#include <sd_handler.h>

bool sdh_init()
{
  // Begin SD library using known pin layout
  bool result = SD.begin(SDH_PIN_CS);

  if (result) {
    dbg_log("SD card slot initialized (type=%" PRIu8 ")!\n", SD.cardType());
  } else
    dbg_log("Could not initialize SD card slot!\n");

  return result;
}

File sdh_open_create_if_not_exists(const char* path, const char* mode)
{
  if (!SD.exists(path))
    SD.open(path, "w").close();
  return SD.open(path, mode);
}

File sdh_open_vars_file(const char* mode)
{
  return sdh_open_create_if_not_exists(SDH_FILE_VARS, mode);
}

File sdh_open_frames_file(const char* mode)
{
  return sdh_open_create_if_not_exists(SDH_FILE_FRAMES, mode);
}