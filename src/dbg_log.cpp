#include <dbg_log.h>

/*
============================================================================
                            Basic debug control                             
============================================================================
*/

void init_dbg_log()
{
  // Only initialize serial communication when in debugging mode
  #if DBGL_ACTIVE
  Serial.begin(DBGL_BAUD);

  // "Flush" out
  Serial.println();
  #endif
}