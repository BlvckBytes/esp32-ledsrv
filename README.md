# esp32-ledsrv

Control addressable LEDs using an ESP32 and serve local frame requests.

## Current status

Working on the absolute basics, trying to get a clean foundation to work off in the future.

## Goals

- [x] Debugging log over serial
- [x] Connect to WiFi
- [x] Automatically reconnect on connection losses
- [x] Open AP when STA connect failed multiple times in a row (but keep on trying)
- [X] Binary WebSocket server
- [X] Request handler switching on OpCodes
- [X] Serving write requests
- [X] Serving read requests
- [ ] Central event system
- [ ] Frame/Pixel handler
- [X] SD card JSON for variables
- [ ] SD card BINARY for frames
- [X] SD card hotplug
- [ ] SD card handle wrong FS
- [ ] Calculate remaining space for BIN file and auto-calc max frame slots
- [X] Cancel processing invalid (segmented) requests ASAP
- [ ] Client authentication
- [ ] Protect against MITM

... and many more.

## ToDo

- [ ] Structure large files into sections using block comments

## Documentation

### Request Structure

Requests are basically containing a list of bytes, sent as a binary buffer. They are formatted like this:

`<8b opcode><optional command data>`

Responses are made up similarly:

`<8b resultcode><optional result data>`

Frame indices, frame durations and string lengths are always represented by two bytes, for future proofness SD card MBs are four bytes.

As of now, all other data is byte-based.

Events send data to the client if their channel has been subscribed beforehand.

Known eventcodes:

* `0x00` Frame duration changed
* `0x01` Active number of frames changed
* `0x02` Frame content deployed
* `0x03` Total brightness changed
* `0x04` WiFi credentials changed
* `0x05` SD card availability changed

Known resultcodes:

* `0xFF` Success, data is the following bytes
* `0x00` Success, no data
* `0x01` Text message received
* `0x02` Argument(s) missing
* `0x03` Too little/too many pixels
* `0x04` Frame index out of range
* `0x05` Pixel color out of range
* `0x06` Brightness out of range
* `0x07` String not terminated
* `0x08` Empty request
* `0x09` Could not access SD card
* `0x0A` Fragmented requests are not supported
* `0x0B` Message longer than internal buffer
* `0x0C` Unknown opcode requested
* `0x0D` String argument too long
* `0x0E` Too many string arguments

Known opcodes:

0x00-0x7F is setting and 0x80-0xFF is getting data.

* `0x00` Set frame duration in ms
  * Fmt: `<0x00><duration uint_16t>`
  * Res: `0x00` | `0x01` | `0x02`
* `0x01` Set active number of frames
  * Fmt: `<0x01><num_frames uint_16t>`
  * Res: `0x00` | `0x01` | `0x02` | `0x04`
* `0x02` Set frame content by index in framebuffer
  * Fmt: `<0x02><frame_index uint16_t>(<R uint8_t><G uint8_t><B uint8_t>)*num_pixels`
  * Res: `0x00` | `0x01` | `0x02` | `0x03` | `0x04` | `0x05` | `0x09`
* `0x03` Set total brightness
  * Fmt: `<0x03><brightness uint8_t>`
  * Res: `0x00` | `0x01` | `0x02`| `0x06`
* `0x04` Set WiFi credentials
  * Fmt: `<0x04>(<0x00-0xFF>)*ssid_strlen(<0x00-0xFF>)*pass_strlen`
  * Res: `0x00` | `0x01` | `0x02` | `0x07`
* `0x05` Set event subscription state
  * Fmt: `<0x05><eventcode uint8_t><state_bool uint8_t>`
  * Res: `0x00` | `0x01` | `0x02` | `0x08`
* `0x80` Get frame duration in ms
  * Fmt: `<0x80>`
  * Res: `<0xFF><0x0000-0xFFFF>`
* `0x81` Get active number of frames
  * Fmt: `<0x81>`
  * Res: `<0xFF><0x0000-0xFFFF>`
* `0x82` Get available number of frame slots
  * Fmt: `<0x82>`
  * Res: `<0xFF><0x0000-0xFFFF>` | `0x09`
* `0x83` Get frame content by index
  * Fmt: `<0x83><frame_index uint16_t>`
  * Res: `<0xFF>(<8b R><8b G><8b B>)*num_pixels` | `0x04` | `0x09`
* `0x84` Get total brightness
  * Fmt: `<0x84>`
  * Res: `<0xFF><0x00-0xFF>`
* `0x85` Get current SSID
  * Fmt: `<0x85>`
  * Res: `<0xFF>(<0x00-0xFF>)*ssid_strlen`
* `0x86` Get SD card total size in MB
  * Fmt: `<0x86>`
  * Res: `<0xFF><total_size uint32_t>` | `0x09`

### Event Structure

Events are the only way the server can communicate with it's clients. Packets are formatted in this manner:

`<8b eventcode><optional event data>`