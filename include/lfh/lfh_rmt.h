#ifndef lfh_rmt_h
#define lfh_rmt_h

#include <stddef.h>
#include <inttypes.h>
#include <esp32-hal-rmt.h>
#include <driver/rmt.h>
#include <dbg_log.h>
#include <variable_store.h>

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

/**
 * @brief Initialize the RMT transmission buffer with standard values
 */
void lfh_rmt_tx_data_init();

/**
 * @brief Allocate the RMT transmission buffer
 */
void lfh_rmt_tx_data_alloc();

/**
 * @brief Free the RMT transmission buffer
 */
void lfh_rmt_dealloc();

/**
 * @brief Initialize the RMT devic
 */
void lfh_rmt_init();

/**
 * @brief Copy a frame of pixel bytes into the RMT transmission buffer
 * 
 * @param frame_data Frame data
 * @param frame_size Frame size
 */
void lfh_rmt_copy_frame(uint8_t *frame_data, uint32_t frame_size);

/**
 * @brief Write out the items from the transmission buffer onto the data line
 * 
 * @param async Whether or not to block
 */
void lfh_rmt_write_items(bool async);

#endif