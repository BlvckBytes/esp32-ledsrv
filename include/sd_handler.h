#ifndef sd_handler_h
#define sd_handler_h

#include <dbg_log.h>
#include <inttypes.h>
#include <SPI.h>
#include <SD.h>

// SD card hardware config
#define SDH_PIN_CS 5

// SD card file config
#define SDH_FILEPREF "esp32_ledsrv_"
#define SDH_FILE_VARS "/" SDH_FILEPREF "variables.json"
#define SDH_FILE_FRAMES "/" SDH_FILEPREF "frames.bin"

// Reserved size for variables file
#define SDH_FILESZ_VARS 1048576 // 1MB

/**
 * @brief Initialize the SD card slot
 */
void sdh_init();

/**
 * @brief Open the frames file
 * 
 * @param mode Access mode (r/w)
 * @return File File handle
 */
File sdh_open_frames_file(const char* mode);

/**
 * @brief Open the variables file
 * 
 * @param mode Access mode (r/w)
 * @return File File handle
 */
File sdh_open_vars_file(const char* mode);

#endif