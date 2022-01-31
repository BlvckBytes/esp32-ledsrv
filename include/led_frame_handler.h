#ifndef led_frame_handler_h
#define led_frame_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <inttypes.h>
#include <dbg_log.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// WARNING: This value needs to be calculated at runtime depending on the SD remaining size, clamped to a max of uint16_t
#define MAX_FRAMES 180

/*
============================================================================
                             Framebuffer info                               
============================================================================
*/

uint16_t lfh_get_frame_slots();

#endif