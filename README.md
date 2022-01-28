# esp32-ledsrv

Control addressable LEDs using an ESP32 and serve local frame requests.

## Current status

Working on the absolute basics, trying to get a clean foundation to work off in the future.

## Goals

- [x] Debugging log over serial
- [x] Connect to WiFi
- [x] Automatically reconnect on connection losses
- [ ] Binary WebSocket server
- [ ] Request handler switching on OpCodes
- [ ] Serving write requests
- [ ] Serving read requests
- [ ] Frame/Pixel handler
- [ ] Cancel processing invalid (segmented) requests ASAP
- [ ] Client authentication
- [ ] Protect against MITM

... and many more.

## Documentation

### Request Structure

Requests are basically containing a list of bytes, sent as a binary buffer. They are formatted like this:

`<8b opcode><optional command data>`

Responses are made up similarly:

`<8b resultcode><optional result data>`

Frame indices as well as frame durations are always represented by two bytes, for future proofness. As of now, all other data is byte-based.

Known resultcodes:

* `0xFF` Success, data is the following bytes
* `0x00` Success, no data
* `0x01` Text message received
* `0x02` Argument missing
* `0x03` Too little/too many pixels
* `0x04` Frame index out of range
* `0x05` Pixel color out of range
* `0x06` Brightness out of range

Known opcodes:

0x00-0x7F is setting and 0x80-0xFF is getting data.

* `0x00` Set frame duration in ms
  * Req: `<0x00><0x05>` 5ms/frame
  * Res: `0x00` | `0x01` | `0x02`
* `0x01` Set active number of frames
  * Req: `<0x01><0x04>` 4 active frames total
  * Res: `0x00` | `0x01` | `0x02` | `0x04`
* `0x02` Set frame content by index in framebuffer
  * Req: `<0x02><0x0000-0xFFFF>(<8b R><8b G><8b B>)*NUM_PIXELS` Set frame
  * Res: `0x00` | `0x01` | `0x02` | `0x03` | `0x04` | `0x05`
* `0x03` Set total brightness
  * Req: `<0x03><0x00-0xFF>`
  * Res: `0x00` | `0x01` | `0x02`| `0x06`
* `0x80` Get frame duration in ms
  * Req: `<0x80>`
  * Res: `<0xFF><0x0000-0xFFFF>`
* `0x81` Get active number of frames
  * Req: `<0x81>`
  * Res: `<0xFF><0x0000-0xFFFF>`
* `0x82` Get available number of frame slots
  * Req: `<0x82>`
  * Res: `<0xFF><0x0000-0xFFFF>`
* `0x83` Get frame content by index
  * Req: `<0x83><0x0000-0xFFFF>`
  * Res: `<0xFF>(<8b R><8b G><8b B>)*NUM_PIXELS` | `0x04`
* `0x84` Get total brightness
  * Req: `<0x84>`
  * Res: `<0xFF><0x00-0xFF>`
