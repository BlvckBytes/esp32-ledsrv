#include <lfh/lfh_pause.h>

static PauseRequest lfh_pause_reqs[LFH_PAUSE_CB_RINGBUF_SIZE] = { { 0, 0 } };
static size_t lfh_pause_reqs_index;
static int lfh_pause_timeout_handle = -1;
static lfh_resume_t lfh_resume_cb;
static bool lfh_paused;

void lfh_pause_timeout_cb()
{
  lfh_paused = false;
  if (lfh_resume_cb) lfh_resume_cb();
}

/**
 * @brief Pulses the timeout timer to reset it
 */
void lfh_pause_timeout_pulse()
{
  if (!toh_is_active(lfh_pause_timeout_handle))
    lfh_pause_timeout_handle = toh_create_timeout(LFH_PAUSE_TIMEOUT_MS, lfh_pause_timeout_cb);
  toh_reset(lfh_pause_timeout_handle);
}

bool lfh_is_paused()
{
  return lfh_paused;
}

void lfh_pause_register(lfh_pause_req_t avail, void *arg)
{
  // Already paused, call immediately
  if (lfh_paused)
  {
    avail(arg);
    lfh_pause_timeout_pulse();
    return;
  }

  lfh_pause_reqs[lfh_pause_reqs_index++] = {avail, arg};

  // Wrap index around buffer
  if (lfh_pause_reqs_index == LFH_PAUSE_CB_RINGBUF_SIZE)
    lfh_pause_reqs_index = 0;
}

bool lfh_pause_check()
{
  for (size_t i = 0; i < LFH_PAUSE_CB_RINGBUF_SIZE; i++)
  {
    PauseRequest req = lfh_pause_reqs[i];

    // Slot not active
    if (!req.cb) continue;

    // At least one waiting
    return true;
  }

  // Currently no requests pending
  return false;
}

void lfh_pause_call(lfh_resume_t timeout)
{
  lfh_resume_cb = timeout;
  lfh_pause_timeout_pulse();
  lfh_paused = true;

  for (size_t i = 0; i < LFH_PAUSE_CB_RINGBUF_SIZE; i++)
  {
    PauseRequest *req = &lfh_pause_reqs[i];

    // Slot not active
    if (!req->cb) continue;

    // Invalidate and call
    lfh_pause_req_t cb = req->cb;
    req->cb = NULL;
    cb(req->arg);
  }
}