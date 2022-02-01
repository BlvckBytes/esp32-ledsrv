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

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// WARNING: This value needs to be calculated at runtime depending on the SD remaining size, clamped to a max of uint16_t
#define MAX_FRAMES 180
#define MAX_PIXELS 1024

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

/*
============================================================================
                             Framebuffer write                              
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
 * @brief Write the data of a frame to the persistent frame buffer
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
                             Framebuffer read                               
============================================================================
*/

/**
 * @brief Read the frame from persistent data into a buffer
 * 
 * @param frame_index Zero based index of the frame
 * @param out_buf Buffer to store data in
 * 
 * @return true Successfully read into buffer
 * @return false Data not available atm
 */
bool lfh_read_frame(uint16_t frame_index, uint8_t *out_buf);

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