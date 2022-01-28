#include <Arduino.h>
#include <dbg_log.h>
#include <wifi_handler.h>

// Network credentials
// INFO: This should later reside within EEPROM
#define WL_SSID "handygurkn_2"
#define WL_PASS "unogetinhea"
#define WL_TIMEOUT 15000

void setup() {
  // Set up debugging capabilities
  init_dbg_log(115200);

  // Initial network connect attempt
  wlh_connect_ap_dhcp(WL_SSID, WL_PASS, WL_TIMEOUT);
}

void loop() {
  // Reconnect if connection broke for some reason
  if (!wlh_is_connected()) {
    dbg_log("Reconnect initialized!\n");
    wlh_connect_ap_dhcp(WL_SSID, WL_PASS, WL_TIMEOUT);
  }

  delay(200);
}