#include <led_frame_handler.h>
#include <dbg_log.h>

// INFO: These variables will later reside on the SD card
uint16_t lfh_frame_duration = 0;
uint16_t lfh_num_frames = 0;
uint8_t lfh_brightness = 0;
const char* lfh_ssid = "";
const char* lfh_password = "";

#define MAX_FRAMES 180

void lfh_set_frame_duration(uint16_t duration)
{
  lfh_frame_duration = duration;
  dbg_log("Set frame duration variable to %" PRIu16 "!\n", duration);
}

void lfh_set_num_frames(uint16_t num_frames)
{
  lfh_num_frames = num_frames;
  dbg_log("Set num frames variable to %" PRIu16 "!\n", num_frames);
}

void lfh_set_brightness(uint8_t brightness)
{
  lfh_brightness = brightness;
  dbg_log("Set brightness variable to %" PRIu8 "!\n", brightness);
}

void lfh_set_wifi_credentials(char* ssid, char* password){
  lfh_ssid = ssid;
  lfh_password = ssid;
  dbg_log("Set wifi credentials to (%s, %s)!\n", ssid, password);
}

uint16_t lfh_get_frame_duration()
{
  return lfh_frame_duration;
}

uint16_t lfh_get_num_frames()
{
  return lfh_num_frames;
}

uint16_t lfh_get_frame_slots()
{
  return MAX_FRAMES;
}

uint8_t lfh_get_brightness()
{
  return lfh_brightness;
}

const char* lfh_get_wifi_ssid()
{
  return lfh_ssid;
}