#include <sd_handler.h>

/*
============================================================================
                              Basic SD control                              
============================================================================
*/

static bool sdh_avail = false;

/**
 * @brief Setter interceptor for sdh_avail
 * 
 * @param state State to set the variable to
 */
void sdh_set_avail(bool state)
{
  bool prev = sdh_avail;
  sdh_avail = state;

  // Only fire event on delta
  if (prev != state)
    evh_fire_event(0, SD_CARD_STATE);
}

bool sdh_init()
{
  // Begin SD library using known pin layout
  if (SD.begin(SDH_PIN_CS)) {
    dbg_log("SD card slot initialized (type=%" PRIu8 ")!\n", SD.cardType());
    sdh_set_avail(true);
  } else {
    dbg_log("Could not initialize SD card slot!\n");
    sdh_set_avail(false);
  }

  return sdh_avail;
}

/*
============================================================================
                               SD information                               
============================================================================
*/

bool sdh_io_available()
{
  return sdh_avail;
}

uint32_t sdh_get_total_size_mb()
{
  if (!sdh_avail) return 0;
  return sdh_bytes_to_mb(SD.totalBytes());
}

/*
============================================================================
                            Hotplug capabilities                            
============================================================================
*/

static long sdh_last_hotplug_watch = millis();

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
  // WARNING: This took a patch:
  // WARNING: * Add sdCommand to sd_diskio.h
  uint32_t resp = UINT32_MAX;
  sdCommand(SDH_PDRV, 13, 0, &resp);

  // Couldn't answer, thus not available
  if (resp == UINT32_MAX)
  {
    sdh_set_avail(false);
    SD.end();
    dbg_log("De-inited SD!\n");
    return;
  }

  sdh_set_avail(true);
}

/*
============================================================================
                                File handles                                
============================================================================
*/

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