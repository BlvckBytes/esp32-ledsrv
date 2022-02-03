#include <timeout_handler.h>

TimeoutEntry toh_registry[TOH_REGISTRY_SLOTS] = { 0, 0, 0 };
static uint16_t toh_registry_index = 0;

int toh_create_timeout(uint32_t duration_ms, callback cb)
{
  uint16_t recv_index = toh_registry_index;

  toh_registry[recv_index] = {
    duration_ms,
    millis() + duration_ms,
    cb
  };

  toh_registry_index++;
  return recv_index;
}

void toh_reset(int handle)
{
  if (handle < 0 || handle >= TOH_REGISTRY_SLOTS) return;
  TimeoutEntry *entry = &toh_registry[handle];
  entry->stamp = millis() + entry->duration;
}

bool toh_is_active(int handle)
{
  if (handle < 0 || handle >= TOH_REGISTRY_SLOTS) return false;
  TimeoutEntry *entry = &toh_registry[handle];
  return entry->duration != 0 && entry->stamp > millis();
}

void toh_check()
{
  for (int i = 0; i < TOH_REGISTRY_SLOTS; i++)
  {
    TimeoutEntry *entry = &toh_registry[i];

    if (entry->duration == 0 || entry->stamp > millis()) continue;

    if (entry->cb) entry->cb();
    dbg_log("Disabled timeout with handle %d and duration %" PRIu32 "!\n", i, entry->duration);
    entry->duration = 0;
  }
}