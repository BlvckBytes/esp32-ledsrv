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

/*
============================================================================
                                  Macros                                    
============================================================================
*/

#define LFH_MAX_PIXELS 1024

// How many slots the ringbuffer storing frame data should have
#define LFH_FRAME_RINGBUF_SLOTS 2

// INFO: Hack test value, for now...
#define LHF_CONST_FRAME_TIME 10

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
 * @brief Read the next frame into the local ringbuffer
 */
bool lfh_read_frame();

/**
 * @brief Handle the current frame to be displayed and advance the index
 */
void lfh_handle_frame();

#endif