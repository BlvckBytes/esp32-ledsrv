#ifndef wifi_handler_h
#define wifi_handler_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <IPAddress.h>
#include <WiFi.h>
#include <variable_store.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// IP address formatting before printing
#define format_ip_addr(ip) ip.toString().c_str()

// Access point config
#define WFH_CONN_TRIALS_UNTIL_AP 2
#define WFH_CONF_AP_PASS NULL

// STA timeouts
#define WFH_TIMEOUT 15000
#define WFH_CONN_STATUS_CACHE 1000
#define WFH_RECONN_COOLDOWN 2000

/*
============================================================================
                               Station Mode                                 
============================================================================
*/

/**
 * @brief Establish a WiFi connection to the station specified in variable
 * store and retrieve the configuration using DHCP
 * @return true Connection successful
 * @return false Could not connect
 */
bool wfh_sta_connect_dhcp();

/**
 * @brief Yields the active state of a current station connection
 * 
 * @return true Active connection exists
 * @return false Device not connected
 */
bool wfh_sta_is_connected();

/**
 * @brief Ensure that an active connection exists, reconnect otherwise
 * 
 * @return true When an active connection is present or when reconnect succeeded
 * @return false When no connection exists and reconnecting failed
 */
bool wfh_sta_ensure_connected();

/**
 * @brief Print the currently active station connection info
 */
void wfh_sta_dbg_conn_info();

/*
============================================================================
                                 AP Mode                                    
============================================================================
*/

/**
 * @brief Create an access point
 * 
 * @param password PSK for auth, null means open
 * @param gateway Gateway
 */
void wfh_ap_create(
  const char* password,

  // INFO: This parameter could allow internet access through AP mode in the future
  IPAddress gateway
);

/**
 * @brief Close the currently active AP, if any
 */
void wfh_ap_close();

/**
 * @brief Print the currently active access point connection info
 */
void wfh_ap_dbg_conn_info();

#endif