#include <Arduino.h>
#include <dbg_log.h>
#include <wifi_handler.h>
#include <web_server_handler.h>
#include <web_socket_handler.h>
#include <sd_handler.h>
#include <variable_store.h>

#include <sd_diskio.h>
#define PRINT_DEL 5000
long last_print = millis();
bool sd_inited = false;

void setup()
{
  // Set up debugging capabilities
  init_dbg_log();

  // Initial network connect attempt
  wfh_connect_sta_dhcp(WFH_SSID, WFH_PASS, WFH_TIMEOUT);

  // Initialize SD card slot
  sd_inited = sdh_init();

  // Patch variables from file into memory
  // vars_patch_from_json_file();

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

  if (millis() - last_print > PRINT_DEL) {
    last_print = millis();

    // Try to init SD now
    if (!sd_inited)
      sd_inited = sdh_init();

    // Don't continue on
    if (!sd_inited) return;

    uint32_t resp = UINT32_MAX;

    // SEND_STATUS = 13, responds with R2 (2 bytes)
    // WARNING: This took two patches:
    // WARNING: * Add sdCommand to sd_diskio.h
    // WARNING: * Make _pdrv public inside SD.h
    sdCommand(SD._pdrv, 13, 0, &resp);

    if (resp == UINT32_MAX)
    {
      sd_inited = false;
      SD.end();
      dbg_log("De-inited SD!\n");
    }

    dbg_log("SD card available: %s\n", resp == UINT32_MAX ? "no" : "yes");
  }
}