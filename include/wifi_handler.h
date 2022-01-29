#ifndef wifi_handler_h
#define wifi_handler_h

#include <IPAddress.h>
#include <WiFi.h>

// IP address formatting before printing
#define format_ip_addr(ip) ip.toString().c_str()

// Access point config
#define WFH_CONN_TRIALS_UNTIL_AP 2
#define WFH_CONF_AP_SSID "ESP32_LedSrv"
#define WFH_CONF_AP_PASS NULL

// STA network config
// INFO: This should later reside within EEPROM
#define WFH_SSID "HL_HNET_2"
#define WFH_PASS "mysql2001"

// STA timeouts
#define WFH_TIMEOUT 15000
#define WFH_CONN_STATUS_CACHE 1000
#define WFH_RECONN_COOLDOWN 2000

// Device information
#define WFH_HOSTN "ESP32_LedSrv"

/**
 * @brief Establish a WiFi connection to the specified station and retrieve the configuration using DHCP
 * 
 * @param ssid SSID of network
 * @param password PSK for auth
 * @param timeout Timeout in milliseconds
 * @return true Connection successful
 * @return false Could not connect
 */
bool wfh_connect_sta_dhcp(const char* ssid, const char* password, long timeout);

/**
 * @brief Establish a WiFi connection to the specified station using a static configuration
 * TODO: Implement
 * 
 * @param ssid SSID of network
 * @param password PSK for auth
 * @param timeout Timeout in milliseconds
 * @param local_ip IP address of the device
 * @param subnet Subnetmask
 * @param gateway Gateway
 * @param dns1 First DNS server
 * @param dns2 Second DNS server
 * @return true Connection successful
 * @return false Could not connect
 */
bool wfh_connect_sta_static(
  const char* ssid,
  const char* password,
  long timeout,
  IPAddress local_ip,
  IPAddress subnet,
  IPAddress gateway,
  IPAddress dns1,
  IPAddress dns2
);

/**
 * @brief Create an access point
 * 
 * @param ssid SSID to be discovered/connected by
 * @param password PSK for auth
 * @param gateway Gateway
 */
void wfh_create_ap(
  const char* ssid,
  const char* password,

  // INFO: This parameter could allow internet access through AP mode in the future
  IPAddress gateway
);

/**
 * @brief Close the currently active AP, if any
 */
void wfh_close_ap();

/**
 * @brief Yields the active state of a current station connection
 * 
 * @return true Active connection exists
 * @return false Device not connected
 */
bool wfh_is_connected();

/**
 * @brief Print the currently active access point connection info
 */
void wfh_dbg_ap_conn_info();

/**
 * @brief Ensure that an active connection exists, reconnect otherwise
 * 
 * @return true When an active connection is present or when reconnect succeeded
 * @return false When no connection exists and reconnecting failed
 */
bool wfh_ensure_connected();

#endif