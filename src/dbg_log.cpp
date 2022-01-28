#include <dbg_log.h>

void init_dbg_log(int baud) {
  // Only initialize serial communication when in debugging mode
  #if DEBUG_MODE_ACTIVE
  Serial.begin(baud);

  // "Flush" out
  Serial.println();
  #endif
}