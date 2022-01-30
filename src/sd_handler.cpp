#include <sd_handler.h>

static bool sdh_avail = false;
static long sdh_last_hotplug_watch = millis();

bool sdh_init()
{
  // Begin SD library using known pin layout
  if (SD.begin(SDH_PIN_CS)) {
    dbg_log("SD card slot initialized (type=%" PRIu8 ")!\n", SD.cardType());
    sdh_avail = true;
  } else {
    dbg_log("Could not initialize SD card slot!\n");
    sdh_avail = false;
  }

  return sdh_avail;
}

bool sdh_io_available()
{
  return sdh_avail;
}

void sdh_watch_hotplug()
{
  // Watch interval timer
  if (millis() - sdh_last_hotplug_watch < SDH_HOTPLUG_WATCH_DEL) return;
  sdh_last_hotplug_watch = millis();

  // Try to init SD now
  if (!sdh_avail)
    // Still not accessible, try again at next iteration
    if (!sdh_init()) return;

  // CMD SEND_STATUS = 13, responds with R2 (2 bytes)
  // WARNING: This took two patches:
  // WARNING: * Add sdCommand to sd_diskio.h
  // WARNING: * Make _pdrv public inside SD.h
  uint32_t resp = UINT32_MAX;
  sdCommand(SD._pdrv, 13, 0, &resp);

  // Couldn't answer, thus not available
  if (resp == UINT32_MAX)
  {
    sdh_avail = false;
    SD.end();
    dbg_log("De-inited SD!\n");
    return;
  }

  sdh_avail = true;
}

bool sdh_open_create_if_not_exists(
  const char* path,
  const char* mode,
  File *f_out
)
{
  // SD card not available
  if (!sdh_avail) return false;

  // Create file if not yet existing
  if (!SD.exists(path))
    SD.open(path, "w").close();

  // Set buffer variable
  *f_out = SD.open(path, mode);
  return true;
}

bool sdh_open_vars_file(const char* mode, File *f_out)
{
  return sdh_open_create_if_not_exists(SDH_FILE_VARS, mode, f_out);
}

bool sdh_open_frames_file(const char* mode, File *f_out)
{
  return sdh_open_create_if_not_exists(SDH_FILE_FRAMES, mode, f_out);
}