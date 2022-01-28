#include <Arduino.h>
#include <dbg_log.h>
#include <wifi_handler.h>

bool ensure_connected()
{
  if (!wfh_is_connected())
  {
    // Reconnect if connection broke for some reason
    dbg_log("Reconnect initialized!\n");
    bool success = wfh_connect_sta_dhcp(WFH_SSID, WFH_PASS, WFH_TIMEOUT);

    // Successful reconnect, continue program
    if (success) return true;

    // Unsuccessful, delay between trials
    delay(WFH_RECONN_DEL);
    return false;
  }

  // Active connection exists
  return true;
}

void setup()
{
  // Set up debugging capabilities
  init_dbg_log(115200);

  // Initial network connect attempt
  wfh_connect_sta_dhcp(WFH_SSID, WFH_PASS, WFH_TIMEOUT);
}

void loop()
{
  if (!ensure_connected()) return;
}