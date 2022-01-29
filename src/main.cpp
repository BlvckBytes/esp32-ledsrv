#include <Arduino.h>
#include <dbg_log.h>
#include <wifi_handler.h>
#include <web_server_handler.h>
#include <web_socket_handler.h>

void setup()
{
  // Set up debugging capabilities
  init_dbg_log();

  // Initial network connect attempt
  wfh_connect_sta_dhcp(WFH_SSID, WFH_PASS, WFH_TIMEOUT);

  // Initialize web server
  wsrvh_init();

  // Initialize websocket server on top of web server
  wsockh_init();
}

void loop()
{
  if (!wfh_ensure_connected()) return;

  // Clean up web socket server
  wsockh_cleanup();
}