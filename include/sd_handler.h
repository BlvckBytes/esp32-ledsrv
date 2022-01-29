#ifndef sd_handler_h
#define sd_handler_h

#include <dbg_log.h>
#include <inttypes.h>
#include <SPI.h>
#include <SD.h>

// SD card hardware config
#define SDH_PIN_CS 5

/**
 * @brief Initialize the SD card slot
 */
void sdh_init();

#endif