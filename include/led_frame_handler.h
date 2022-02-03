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
#include <esp32-hal-rmt.h>
#include <Adafruit_NeoPixel.h>
#include <driver/rmt.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// WARNING: This value needs to be calculated at runtime depending on the SD remaining size, clamped to a max of uint16_t
#define LFH_MAX_FRAMES 180
#define LFH_MAX_PIXELS 1024

// Data signal output pin, connected to => DIN =>
#define LFH_LED_DATA_PIN 13

/*
     <----T0H---><------T0L------->
     +----------+                 +-----
     |          |                 |
     |          |                 |
     |          |                 |
     |          |                 |
_____+          +-----------------+

  T0H ... High time of signal representing 0
  T0L ... Low time of signal representing 0
*/
#define LFH_LED_DUR_T0H_US 4
#define LFH_LED_DUR_T0L_US 8

/*
     <------T1H-------><----T1L--->
     +-----------------+          +-----
     |                 |          |
     |                 |          |
     |                 |          |
     |                 |          |
_____+                 +----------+

  T1H ... High time of signal representing 1
  T1L ... Low time of signal representing 1
*/
#define LFH_LED_DUR_T1H_US 8
#define LFH_LED_DUR_T1L_US 4

// This divides the APB clock to be used as a timebase for RMT
// APB should clock at 80 MHz, thus 8 would yield:
// 80 MHz / 8 = 10 MHz, MHz = 1/t => t = 1 / (80 * 10^6 / 8) => 1 DUR ≙ 100 ns
#define LFH_LED_CLK_DIV 8

// INFO: Should be able to hold a full frame, when MAX_PIXELS has been reached
// INFO: Will be read in blocks of 512 bytes
#define LFH_FREAD_BUF_SIZE (512 * 6)

// How many slots the ringbuffer storing frame data should have
#define LFH_FRAME_RINGBUF_SLOTS 2

// Block size of frame ringbuffer
#define LFH_FRAME_RINGBUF_BS 512

/*
============================================================================
                               Basic control                                
============================================================================
*/

/**
 * @brief Initialize the frame handlers dependencies
 */
void lfh_init();

/**
 * @brief Deinitialize the frame buffer and close it's dependencies
 */
void lfh_deinit();

/**
 * @brief Pauses frame processing
 */
void lfh_pause();

/**
 * @brief Resumes frame processing
 */
void lfh_resume();

/*
============================================================================
                             Frame file write                               
============================================================================
*/

/**
 * @brief Create a brand new file according to the current number of
 * pixels and the number of frames, containing only zeros (all pixels off)
 * 
 * @return true File successfully created
 * @return false Could not create file
 */
bool lfh_init_file();

/**
 * @brief Write the data of a frame to the persistent frame file
 * 
 * @param frame_index Zero based index of the frame
 * @param frame_data Data to be written
 * 
 * @return true Successfully written
 * @return false File could not be opened
 */
bool lfh_write_frame(uint16_t frame_index, uint8_t *frame_data);

/*
============================================================================
                             Frame file read                                
============================================================================
*/

/**
 * @brief Read the frame from file at current location into the ringbuffer
 * 
 * @return true Successfully read into buffer
 * @return false Data not available atm
 */
bool lfh_read_frame();

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

/**
 * @brief Get the number of available frame slots which can hold frame data
 * 
 * @return uint16_t Number of total slots
 */
uint16_t lfh_get_frame_slots();

/**
 * @brief Get the size of an individual frame
 * 
 * @return uint16_t Size in bytes
 */
uint16_t lfh_get_frame_size();

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
 * @brief Handle the current frame to be displayed and advance the index
 */
void lfh_handle_frame();

#endif