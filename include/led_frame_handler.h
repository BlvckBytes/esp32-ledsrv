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
 * @brief Resizes the frames file according to the current number of frames
 */
void lfh_resize_file();

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
 * @brief Get the number of frames, capped by the storage capabilities
 * 
 * @return uint16_t Number of frames
 */
uint16_t lfh_get_num_frames_capped();

/**
 * @brief Get the size of an individual frame
 * 
 * @return uint16_t Size in bytes
 */
uint16_t lfh_get_frame_size();

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