/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <Arduino.h>
#include <wifi_handler.h>
#include <reboot_handler.h>
#include <web_server_handler.h>
#include <web_socket_handler.h>
#include <sd_handler.h>
#include <variable_store.h>

/*
============================================================================
                               Initialization                               
============================================================================
*/

void setup()
{
  // Set up debugging capabilities
  init_dbg_log();

  // Initialize SD card slot
  sdh_init();

  // Patch variables from file into memory
  vars_patch_from_json_file();

  // Initial network connect attempt
  wfh_sta_connect_dhcp();

  // Initialize web server
  wsrvh_init();

  // Initialize websocket server on top of web server
  wsockh_init();
}

/*
============================================================================
                                  Main loop                                 
============================================================================
*/

void loop()
{
  // // Ensure an active connection (as far as possible)
  // if (!wfh_sta_ensure_connected())
  // {
  //   // INFO: Put wifi critical calls here
  // }

  // Check for reboot requests
  rbh_check_requests();

  // Clean up web socket server
  wsockh_cleanup();

  // Watch for SD card remove/insert
  sdh_watch_hotplug();
}