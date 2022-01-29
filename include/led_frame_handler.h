#ifndef led_frame_handler_h
#define led_frame_handler_h

#include <inttypes.h>

void lfh_set_frame_duration(uint16_t duration);

void lfh_set_num_frames(uint16_t num_frames);

void lfh_set_brightness(uint8_t brightness);

void lfh_set_wifi_credentials(char* ssid, char* password);

uint16_t lfh_get_frame_duration();

uint16_t lfh_get_num_frames();

uint16_t lfh_get_frame_slots();

uint8_t lfh_get_brightness();

const char* lfh_get_wifi_ssid();

#endif