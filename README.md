# ESP32 BLE Multi Touch Emulator library

This library allows you to make the ESP32 act as a Bluetooth Multi Touch Emulator and control what it does. E.g. tap and release.

## Installation
- (Make sure you can use the ESP32 with the Arduino IDE. [Instructions can be found here.](https://github.com/espressif/arduino-esp32#installation-instructions))
- In the Arduino IDE go to "Sketch" -> "Include Library" -> "Add .ZIP Library..." and select the file you downloaded.
- You can now go to "File" -> "Examples" -> "ESP32 BLE Multi Touch" and select any of the examples to get started.

## Compatibility
- Compatible with Android
- NON-Compatible with iOS nor iPadOS

## Examples

```
#include <BleMultiTouch.h>

BleMultiTouch bleMultiTouch;

void setup() {
  bleMultiTouch.begin();
}

void loop() {
  if (bleMultiTouch.isConnected()) {
    bleMultiTouch.tap(0, 5000, 5000);
  }
  delay(2000);
}
```

Check [examples](examples) for concrete examples.

## API docs

```
// Use 0 - 4 to represent
//   i: id of five different fingers
// Use 0 - 10000 to represent:
//   x: from left to right
//   y: from top to bottom
bleMultiTouch.tap(i,x,y);

// If you want to tap and stay, use touch instead:
bleMultiTouch.touch(i,x,y);

// Since touch() keeps the finger as pressed, you need to release it when convenient
bleMultiTouch.release(i);
```

There is also Bluetooth specific information that you can use (optional):

Instead of `BleMultiTouch bleMultiTouch;` you can do `BleMultiTouch bleMultiTouch("Bluetooth Device Name", "Bluetooth Device Manufacturer", 100);`.

The third parameter is the initial battery level of your device. To adjust the battery level later on you can simply call e.g. `bleMultiTouch.setBatteryLevel(50)` (set battery level to 50%).
By default the battery level will be set to 100%, the device name will be `ESP32 Bluetooth Multi Touch` and the manufacturer will be `Espressif`.

## Credits

Credits to the following individuals as this library is based on their work.
|   |   |
|---|---|
| [T-vK](https://github.com/T-vK)| [ESP32-BLE-Mouse](https://github.com/T-vK/ESP32-BLE-Mouse) |
| [sobrinho](https://github.com/sobrinho) | [ESP32-BLE-Abs-Mouse](https://github.com/sobrinho/ESP32-BLE-Abs-Mouse) |
| [erictroebs](https://github.com/erictroebs) | [Touch.cpp](https://gist.github.com/erictroebs/3e6ca8aa2b9ed06e0b5527fd38dd2c2f) |