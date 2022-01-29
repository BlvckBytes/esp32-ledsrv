#ifndef variable_store_h
#define variable_store_h

#include <inttypes.h>
#include <dbg_log.h>
#include <ArduinoJson.h>
#include <FS.h>

// Variable store default values
#define VARS_DEF_FRAME_DUR 10
#define VARS_DEF_NUM_FRAMES 1
#define VARS_DEF_BRIGHTNESS 255
#define VARS_DEF_WIFI_SSID ""
#define VARS_DEF_WIFI_PASS ""

// Patch variable store from JSON document helper macro
#define vars_patch_var_if_exists(doc, var) if (doc.containsKey(#var)) vars_get()->var = json_doc[#var]

typedef struct VariableStore
{
  uint16_t frame_dur;
  uint16_t num_frames;
  uint8_t brightness;
  const char* wifi_ssid;
  const char* wifi_pass;
} VariableStore;

/**
 * @brief Patch the current store with values read from a JSON file
 */
void vars_patch_from_json_file(File f);

/**
 * @brief Write the current store into a JSON file
 */
void vars_write_to_json_file(File f);

/**
 * @brief Get the current store for R/W access
 */
VariableStore* vars_get();

#endif