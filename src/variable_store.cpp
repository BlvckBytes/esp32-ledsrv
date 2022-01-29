#include <variable_store.h>

// Create store with default values
VariableStore vars_store = {
  VARS_DEF_FRAME_DUR,
  VARS_DEF_NUM_FRAMES,
  VARS_DEF_BRIGHTNESS,
  VARS_DEF_WIFI_SSID,
  VARS_DEF_WIFI_PASS,
};

void vars_dbg_store()
{
  dbg_log(
    "{\n"
    "  frame_dur: %" PRIu16 ",\n"
    "  num_frames: %" PRIu16 ",\n"
    "  brightness: %" PRIu8 ",\n"
    "  wifi_ssid: %s,\n"
    "  wifi_pass: %s\n"
    "}\n",
    vars_store.frame_dur,
    vars_store.num_frames,
    vars_store.brightness,
    vars_store.wifi_ssid,
    vars_store.wifi_pass
  );
}

void vars_patch_from_json_file(File f)
{
  // Read the file into a buffer
  size_t fsz = f.size();
  char contents[fsz];
  for (int i = 0; i < fsz; i++)
    contents[i] = f.read();

  // Deserialize file content
  DynamicJsonDocument json_doc(fsz);
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
  vars_patch_var_if_exists(json_doc, wifi_ssid);
  vars_patch_var_if_exists(json_doc, wifi_pass);

  // Close resource
  f.close();

  dbg_log("Variables store after patching:\n");
  vars_dbg_store();
}

void vars_write_to_json_file(File f)
{
  dbg_log("vars_write_to_json_file");
}

VariableStore* vars_get()
{
  return &vars_store;
}