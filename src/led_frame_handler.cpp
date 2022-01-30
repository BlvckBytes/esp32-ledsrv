#include <led_frame_handler.h>
#include <dbg_log.h>
#include <variable_store.h>

// WARNING: This value needs to be calculated at runtime depending on the SD remaining size, clamped to a max of uint16_t
#define MAX_FRAMES 180

void lfh_set_frame_duration(uint16_t duration)
{
  vars_get()->frame_dur = duration;
  vars_write_to_json_file();
  dbg_log("Set frame duration variable to %" PRIu16 "!\n", duration);
}

void lfh_set_num_frames(uint16_t num_frames)
{
  vars_get()->num_frames = num_frames;
  vars_write_to_json_file();
  dbg_log("Set num frames variable to %" PRIu16 "!\n", num_frames);
}

void lfh_set_brightness(uint8_t brightness)
{
  vars_get()->brightness = brightness;
  vars_write_to_json_file();
  dbg_log("Set brightness variable to %" PRIu8 "!\n", brightness);
}

void lfh_set_wifi_credentials(const char* ssid, const char* password)
{
  vars_get()->wifi_ssid = ssid;
  vars_get()->wifi_pass = password;
  vars_write_to_json_file();
  dbg_log("Set wifi credentials to (\"%s\", \"%s\")!\n", ssid, password);
}

uint16_t lfh_get_frame_duration()
{
  return vars_get()->frame_dur;
}

uint16_t lfh_get_num_frames()
{
  return vars_get()->num_frames;
}

uint16_t lfh_get_frame_slots()
{
  return MAX_FRAMES;
}

uint8_t lfh_get_brightness()
{
  return vars_get()->brightness;
}

const char* lfh_get_wifi_ssid()
{
  return vars_get()->wifi_ssid;
}