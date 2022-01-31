#include <reboot_handler.h>

long rbh_schedule_timestamp = -1;

/*
============================================================================
                                  Scheduler                                 
============================================================================
*/

void rbh_request_schedule(uint32_t ms_delay)
{
  rbh_schedule_timestamp = millis() + ms_delay;
}

void rbh_check_requests()
{
  // No schedule set yet
  if (rbh_schedule_timestamp < 0) return;

  // Time not yet elapsed
  if (millis() < rbh_schedule_timestamp) return;

  // Reboot now
  ESP.restart();
}