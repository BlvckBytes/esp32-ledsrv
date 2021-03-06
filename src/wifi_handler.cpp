#include <wifi_handler.h>
#include <dbg_log.h>
#include <tcpip_adapter.h>

/*
============================================================================
                               Station Mode                                 
============================================================================
*/

static long wfh_conn_last_check = millis();
static long wfh_last_recon = millis();

bool wfh_sta_connect_dhcp()
{
  // Connection trial counter
  static int connection_trials = 0;

  // Load wifi station info from var store
  dbg_log("Attempting to connect to the STA \"%s\"...\n", vars_get_wifi_ssid());

  // Don't override STA&AP mode
  if (WiFi.getMode() != WIFI_AP_STA)
    WiFi.mode(WIFI_STA);

  // Set hostname and init connection
  WiFi.hostname(vars_get_dev_name());
  WiFi.begin(vars_get_wifi_ssid(), vars_get_wifi_pass());

  // Avait connection
  long started = millis();
  while (!wfh_sta_is_connected())
  {
    // Timed out
    if (millis() - started > WFH_TIMEOUT)
    {
      dbg_log("WiFi connection timed out!\n");
      
      // Trials reached threshold, launch AP
      if (++connection_trials == WFH_CONN_TRIALS_UNTIL_AP)
      {
        dbg_log("Reconnect attemp threshold of %d reached!\n", connection_trials);
        wfh_ap_create(
          WFH_CONF_AP_PASS,
          IPAddress(192, 168, 1, 254)
        );
      }

      return false;
    }
  }

  dbg_log("Received config from DHCP:\n");
  wfh_sta_dbg_conn_info();

  // Connection was a success, reset trials and close AP
  connection_trials = 0;
  wfh_ap_close();

  return true;
}

bool wfh_sta_is_connected()
{
  static bool result_cache = false;

  // Return cached value
  if (millis() - wfh_conn_last_check < WFH_CONN_STATUS_CACHE)
    return result_cache;

  // Write new value into cache
  result_cache = (
    // Status needs to be connected
    WiFi.status() == WL_CONNECTED &&

    // IP shall not be 0.0.0.0
    WiFi.localIP() != INADDR_NONE
  );

  wfh_conn_last_check = millis();
  return result_cache;
}

bool wfh_sta_ensure_connected()
{
  if (!wfh_sta_is_connected())
  {
    // Skip until cooldown expired
    if (millis() - wfh_last_recon < WFH_RECONN_COOLDOWN)
      return false;

    // Reconnect if connection broke for some reason
    dbg_log("Reconnect initialized!\n");
    bool success = wfh_sta_connect_dhcp();

    // Reactivate cooldown
    wfh_last_recon = millis();

    // Successful reconnect, continue program
    if (success) {
      // Force conn status cache update
      wfh_conn_last_check -= WFH_CONN_STATUS_CACHE;
      return true;
    }

    // Unsuccessful
    return false;
  }

  // Active connection exists
  return true;
}

void wfh_sta_dbg_conn_info()
{
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

/*
============================================================================
                                 AP Mode                                    
============================================================================
*/

void wfh_ap_create(
  const char* password,
  IPAddress gateway
)
{
  // Load device name from var store
  const char *dev_name = vars_get_dev_name();

  // Open AP but still remain looking for stations
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(dev_name);

  WiFi.softAPConfig(
    IPAddress(192, 168, 1, 1),
    gateway,
    IPAddress(255, 255, 255, 0)
  );
  if (WiFi.softAP(dev_name, password))
  {
    dbg_log("AP \"%s\" created!\n", dev_name);
    wfh_ap_dbg_conn_info();
  }
  else
  {
    dbg_log("Could not create AP!\n");
  }
}

void wfh_ap_close()
{
  if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA)
  {
    WiFi.mode(WIFI_STA);
    dbg_log("Disabled AP mode and went back to STA only!\n");
  }
}

void wfh_ap_dbg_conn_info()
{
  // Check if AP is really configured
  if(
    WiFiGenericClass::getMode() != WIFI_MODE_AP &&
    WiFiGenericClass::getMode() != WIFI_MODE_APSTA
  )
  {
    dbg_log("No AP to debug configured!\n");
    return;
  }

  // Get AP iface info
  tcpip_adapter_ip_info_t ip;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip);

  dbg_log(
    "{\n"
    "  local_ip: %s\n"
    "  submet_mask: %s\n"
    "  gateway: %s\n"
    "  ssid: %s\n"
    "  pass: %s\n"
    "}\n",
    format_ip_addr(IPAddress(ip.ip.addr)),
    format_ip_addr(IPAddress(ip.netmask.addr)),
    format_ip_addr(IPAddress(ip.gw.addr)),
    WiFi.softAPSSID().c_str(),
    WFH_CONF_AP_PASS == NULL ? "NULL" : WFH_CONF_AP_PASS
  );
}