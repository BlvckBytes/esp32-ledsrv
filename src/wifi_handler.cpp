#include <wifi_handler.h>
#include <dbg_log.h>

#define format_ip_addr(ip) ip.toString().c_str()

void wlh_dbg_ap_conn_info() {
  dbg_log(
    "{\n"
    "  local_ip: %s\n"
    "  subnet_mask: %s\n"
    "  gateway: %s\n"
    "  dns_1: %s\n"
    "  dns_2: %s\n"
    "}\n",
    format_ip_addr(WiFi.localIP()),
    format_ip_addr(WiFi.subnetMask()),
    format_ip_addr(WiFi.gatewayIP()),
    format_ip_addr(WiFi.dnsIP(0)),
    format_ip_addr(WiFi.dnsIP(1))
  );
}

bool wlh_is_connected() {
  return (
    // Status needs to be connected
    WiFi.status() == WL_CONNECTED &&

    // IP shall not be 0.0.0.0
    WiFi.localIP() != INADDR_NONE
  );
}

bool wlh_connect_ap_dhcp(const char* ssid, const char* password, long timeout) {
  // Connect to the provided station
  dbg_log("Attempting to connect to an AP...\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Avait connection
  long started = millis();
  while (!wlh_is_connected()) {
    // Timed out
    if (millis() - started > timeout) {
      dbg_log("WiFi connection timed out!\n");
      return false;
    }

    // Slow down checking to a sane rate
    delay(100);
  }

  dbg_log("Received config from DHCP:\n");
  wlh_dbg_ap_conn_info();

  // Connection was a success
  return true;
}