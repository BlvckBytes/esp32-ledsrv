#ifndef wifi_handler_h
#define wifi_handler_h

#include <IPAddress.h>
#include <WiFi.h>

/**
 * @brief Establish a WiFi connection to the specified access point and retrieve the configuration using DHCP
 * 
 * @param ssid SSID of network
 * @param password PSK for auth
 * @param timeout Timeout in milliseconds
 * @return true Connection successful
 * @return false Could not connect
 */
bool wlh_connect_ap_dhcp(const char* ssid, const char* password, long timeout);

/**
 * @brief Establish a WiFi connection to the specified access point using a static configuration
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
bool wlh_connect_ap_static(
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
 * TODO: Implement
 * 
 * @param ssid SSID to be discovered/connected by
 * @param password PSK for auth
 * @param local_ip IP address of the device
 * @param subnet Subnetmask
 * @param gateway Gateway
 * @param channel Channel for the AP
 * @param max_clients Max number of connected clients
 * @param hidden Whether or not to broadcast the SSID
 */
void wlh_create_ap(
  const char* ssid,
  const char* password,
  IPAddress local_ip,
  IPAddress subnet,
  IPAddress gateway,
  int channel = 1,
  int max_clients = 4,
  bool hidden = false
);

/**
 * @brief Yields the active state of a current access point connection
 * 
 * @return true Active connection exists
 * @return false Device not connected
 */
bool wlh_is_connected();

/**
 * @brief Print the currently active access point connection info
 */
void wlh_dbg_ap_conn_info();

#endif