#ifndef variable_store_h
#define variable_store_h

/*
============================================================================
                                  Includes                                  
============================================================================
*/

#include <inttypes.h>
#include <sd_handler.h>
#include <dbg_log.h>
#include <ArduinoJson.h>
#include <FS.h>

/*
============================================================================
                                  Macros                                    
============================================================================
*/

// Variable store default values
#define VARS_DEF_FRAME_DUR 10
#define VARS_DEF_NUM_FRAMES 1
#define VARS_DEF_BRIGHTNESS 255
#define VARS_DEF_WIFI_SSID "YourSSID"
#define VARS_DEF_WIFI_PASS "YourPassword"
#define VARS_DEF_DEV_NAME "ESP32_LEDSRV"

// JSON write buffer capacity
#define VARS_JSON_WRITEBUF_CAPA 256
#define VARS_STRVALBUF_SIZE 128

// Patch variable store from JSON document helper macro
#define vars_patch_var_if_exists(doc, var) if (doc.containsKey(#var)) vars_store.var = json_doc[#var]

#define vars_patch_str_if_exists(doc, var) if (doc.containsKey(#var)) strcpy(vars_store.var, json_doc[#var])

// Add variable store value to JSON document
#define vars_add_var_to_json_doc(doc, var) doc[#var] = vars_store.var

/*
============================================================================
                                   Types                                    
============================================================================
*/

typedef struct VariableStore
{
  uint16_t frame_dur;
  uint16_t num_frames;
  uint8_t brightness;
  char wifi_ssid[VARS_STRVALBUF_SIZE];
  char wifi_pass[VARS_STRVALBUF_SIZE];
  char dev_name[VARS_STRVALBUF_SIZE];
} VariableStore;

/*
============================================================================
                                 Read Data                                  
============================================================================
*/

/**
 * @brief Patch the current store with values read from the JSON file
 */
void vars_patch_from_json_file();

/*
============================================================================
                                 Write Data                                 
============================================================================
*/

/**
 * @brief Write the current store into the JSON file
 */
void vars_write_to_json_file();

/*
============================================================================
                              Variable Getters                              
============================================================================
*/

uint16_t vars_get_frame_dur();
uint16_t vars_get_num_frames();
uint16_t vars_get_brightness();
const char* vars_get_wifi_ssid();
const char* vars_get_wifi_pass();
const char* vars_get_dev_name();

/*
============================================================================
                              Variable Setters                              
============================================================================
*/

void vars_set_frame_dur(uint16_t v);
void vars_set_num_frames(uint16_t v);
void vars_set_brightness(uint16_t v);
void vars_set_wifi_ssid(const char* v);
void vars_set_wifi_pass(const char* v);
void vars_set_dev_name(const char* v);

VariableStore vars_get();

#endif