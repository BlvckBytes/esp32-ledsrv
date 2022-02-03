#ifndef sd_handler_h
#define sd_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <dbg_log.h>
#include <sd_diskio.h>
#include <inttypes.h>
#include <event_handler.h>
#include <SPI.h>
#include <SD.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// SD card hardware config
#define SDH_PIN_CS 5
#define SDH_HOTPLUG_WATCH_DEL 500
#define SDH_SPI_FREQ 1000000UL * 8

// Drive ID, 0 should be totally fine as there never
// will be multiple SD cards in this system
#define SDH_PDRV 0

// SD card file config
#define SDH_FILEPREF "esp32_ledsrv_"
#define SDH_FILE_VARS "/" SDH_FILEPREF "variables.json"
#define SDH_FILE_FRAMES "/" SDH_FILEPREF "frames.bin"

// Reserved size for variables file
#define SDH_FILESZ_VARS 1048576 // 1MB

// Conversion utility
#define sdh_bytes_to_mb(bytes) (bytes / 1000 / 1000)

/*
============================================================================
                              Basic SD control                              
============================================================================
*/

/**
 * @brief Initialize the SD card slot
 * 
 * @return true SD initialized
 * @return false No SD card found
 */
bool sdh_init();

/*
============================================================================
                               SD information                               
============================================================================
*/

/**
 * @brief Checks whether or not card I/O is available (connected and inited)
 * 
 * @return true Card is usable
 * @return false Card is unusable
 */
bool sdh_io_available();

/**
 * @brief Get the total size of the currently in-use SD card in megabytes
 * 
 * @return uint8_t Size in megabytes
 */
uint32_t sdh_get_total_size_mb();

/*
============================================================================
                            Hotplug capabilities                            
============================================================================
*/

/**
 * @brief Watches for hotplug events and updates the system accordingly
 */
void sdh_watch_hotplug();

/*
============================================================================
                                File handles                                
============================================================================
*/

/**
 * @brief Open the frames file
 * 
 * @param mode Access mode (r/w)
 * @param f_out File output buffer variable
 * 
 * @return true File could be opened
 * @return false File couldn't be opened
 */
bool sdh_open_frames_file(const char* mode, File *f_out);

/**
 * @brief Open the variables file
 * 
 * @param mode Access mode (r/w)
 * @param f_out File output buffer variable
 * 
 * @return true File could be opened
 * @return false File couldn't be opened
 */
bool sdh_open_vars_file(const char* mode, File *f_out);

/**
 * @brief Delete the frames file from SD
 * 
 * @return true File could be deleted
 * @return false File couldn't be deleted
 */
bool sdh_delete_frames_file();

#endif