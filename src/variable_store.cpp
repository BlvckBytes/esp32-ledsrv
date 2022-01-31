#include <variable_store.h>

/*
============================================================================
                                 Store Var                                  
============================================================================
*/

// Create store with default values
static VariableStore vars_store = {
  VARS_DEF_FRAME_DUR,
  VARS_DEF_NUM_FRAMES,
  VARS_DEF_BRIGHTNESS,
  VARS_DEF_WIFI_SSID,
  VARS_DEF_WIFI_PASS,
  VARS_DEF_DEV_NAME
};

/*
============================================================================
                                 Debugging                                  
============================================================================
*/

void vars_dbg_store()
{
  dbg_log(
    "{\n"
    "  frame_dur: %" PRIu16 ",\n"
    "  num_frames: %" PRIu16 ",\n"
    "  brightness: %" PRIu8 ",\n"
    "  wifi_ssid: \"%s\",\n"
    "  wifi_pass: \"%s\"\n"
    "  dev_name: \"%s\"\n"
    "}\n",
    vars_store.frame_dur,
    vars_store.num_frames,
    vars_store.brightness,
    vars_store.wifi_ssid,
    vars_store.wifi_pass,
    vars_store.dev_name
  );
}

/*
============================================================================
                                 Read Data                                  
============================================================================
*/

void vars_patch_from_json_file()
{
  // Open file on SD card
  File f;
  if (!sdh_open_vars_file("r", &f)) {
    dbg_log("Could not open vars file for variable store patching!\n");
    return;
  }
  
  // File just has been created, write initial state
  if (f.size() == 0)
  {
    dbg_log("Found an empty variables file, wrote initial state!\n");
    vars_write_to_json_file();
    return;
  }

  size_t f_sz = f.size();
  char contents[f_sz];
  for (int i = 0; i < f_sz; i++)
    contents[i] = f.read();

  // Deserialize file content
  DynamicJsonDocument json_doc(f_sz);
  DeserializationError json_err = deserializeJson(json_doc, contents);

  // Log error
  if (json_err)
  {
    dbg_log("Could not parse JSON file!\n");
    dbg_log("Error: %s\n", json_err.c_str());
    return;
  }

  dbg_log("Variables store before patching:\n");
  vars_dbg_store();

  // Patch json value into variable by name
  vars_patch_var_if_exists(json_doc, frame_dur);
  vars_patch_var_if_exists(json_doc, num_frames);
  vars_patch_var_if_exists(json_doc, brightness);
  vars_patch_str_if_exists(json_doc, wifi_ssid);
  vars_patch_str_if_exists(json_doc, wifi_pass);
  vars_patch_str_if_exists(json_doc, dev_name);

  // Close resource
  f.close();

  dbg_log("Variables store after patching:\n");
  vars_dbg_store();
}

/*
============================================================================
                                 Write Data                                 
============================================================================
*/

void vars_write_to_json_file()
{
  // Open file on SD card
  File f;
  if (!sdh_open_vars_file("w", &f)) {
    dbg_log("Could not open vars file for variable store writing!\n");
    return;
  }

  DynamicJsonDocument json_doc(VARS_JSON_WRITEBUF_CAPA);

  // Add all store vars to the doc
  vars_add_var_to_json_doc(json_doc, frame_dur);
  vars_add_var_to_json_doc(json_doc, num_frames);
  vars_add_var_to_json_doc(json_doc, brightness);
  vars_add_var_to_json_doc(json_doc, wifi_ssid);
  vars_add_var_to_json_doc(json_doc, wifi_pass);
  vars_add_var_to_json_doc(json_doc, dev_name);

  // Write buffer to file
  serializeJsonPretty(json_doc, f);

  // Close resource
  f.close();
  dbg_log("Wrote variables store to file!\n");
}

/*
============================================================================
                              Variable Getters                              
============================================================================
*/

uint16_t vars_get_frame_dur()
{
  return vars_store.frame_dur;
}

uint16_t vars_get_num_frames()
{
  return vars_store.num_frames;
}

uint16_t vars_get_brightness()
{
  return vars_store.brightness;
}

const char* vars_get_wifi_ssid()
{
  return vars_store.wifi_ssid;
}

const char* vars_get_wifi_pass()
{
  return vars_store.wifi_pass;
}

const char* vars_get_dev_name()
{
  return vars_store.dev_name;
}

VariableStore vars_get()
{
  return vars_store;
}

/*
============================================================================
                              Variable Setters                              
============================================================================
*/

void vars_set_frame_dur(uint16_t v)
{
  vars_store.frame_dur = v;
  dbg_log("Set frame_dur=%" PRIu16 "!\n", vars_store.frame_dur);
}

void vars_set_num_frames(uint16_t v)
{
  vars_store.num_frames = v;
  dbg_log("Set num_frames=%" PRIu16 "!\n", vars_store.num_frames);
}

void vars_set_brightness(uint16_t v)
{
  vars_store.brightness = v;
  dbg_log("Set brightness=%" PRIu16 "!\n", vars_store.brightness);
}

void vars_set_wifi_ssid(const char* v)
{
  strcpy(vars_store.wifi_ssid, v);
  dbg_log("Set wifi_ssid=%s!\n", vars_store.wifi_ssid);
}

void vars_set_wifi_pass(const char* v)
{
  strcpy(vars_store.wifi_pass, v);
  dbg_log("Set wifi_pass=%s!\n", vars_store.wifi_pass);
}

void vars_set_dev_name(const char* v)
{
  strcpy(vars_store.dev_name, v);
  dbg_log("Set dev_name=%s!\n", vars_store.dev_name);
}