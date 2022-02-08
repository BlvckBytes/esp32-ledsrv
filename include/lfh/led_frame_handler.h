#ifndef led_frame_handler_h
#define led_frame_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <Arduino.h>
#include <inttypes.h>
#include <dbg_log.h>
#include <variable_store.h>
#include <FS.h>
#include <Adafruit_NeoPixel.h>
#include <lfh/lfh_rmt.h>
#include <lfh/lfh_frame_file.h>
#include <freertos/task.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

#define LFH_MAX_PIXELS 1024

// How many slots the ringbuffer storing frame data should have
#define LFH_FRAME_RINGBUF_SLOTS 8

// INFO: Hack test value, for now...
#define LHF_CONST_FRAME_TIME 8

// Drawing will be very fast, process on core 0 which does wifi
#define LFH_DRAW_CORE 0

// File read will take some time, process on core 1 which is mostly on user-tasks
#define LFH_FILE_CORE 1

/*
============================================================================
                               Basic control                                
============================================================================
*/

/**
 * @brief Initialize the frame handlers dependencies
 */
void lfh_init();

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

/**
 * @brief Get the maximum number of possible pixels to be handled
 * 
 * @return uint16_t Maximum number of pixels
 */
uint16_t lfh_get_max_num_pixels();

/*
============================================================================
                              Frame handling                                
============================================================================
*/

/**
 * @brief Fill up the ringbuffer as far as possible in a new task
 */
void lfh_fill_frame_ringbuf();

/**
 * @brief Read the next frame into the local ringbuffer
 * 
 * @param ringbuf_index Index to start writing at inside the ringbuffer
 * @param num_frames Amount of frames to read and save
 */
bool lfh_read_frames(uint64_t ringbuf_index, uint64_t num_frames);

/**
 * @brief Handle the current frame to be displayed and advance the index
 */
void lfh_handle_frame();

#endif