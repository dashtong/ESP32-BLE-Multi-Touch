/**
 * This example turns the ESP32 into a Bluetooth LE Touch that tap every 2 seconds.
 */
#include <BleMultiTouch.h>

BleMultiTouch bleMultiTouch;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMultiTouch.begin();
}

void loop() {
  if (bleMultiTouch.isConnected()) {
    Serial.println("Tap");
    bleMultiTouch.tap(0, 5000, 5000);
  }
  delay(2000);
}
