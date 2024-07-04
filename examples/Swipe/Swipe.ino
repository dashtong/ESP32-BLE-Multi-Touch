/**
 * This example turns the ESP32 into a Bluetooth LE Touch that swipe left  and right every 2 seconds.
 */
#include <BleMultiTouch.h>

BleMultiTouch bleMultiTouch;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMultiTouch.begin();
}

void loop() {
  if(bleMultiTouch.isConnected()) {
    Serial.println("Swipe Left");
    bleMultiTouch.touch(0,6000, 5000);
    delay(50);
    bleMultiTouch.touch(0,4000, 5000); 
    delay(50);
    bleMultiTouch.release(0);
    delay(2000);
    Serial.println("Double Swipe Right");
    bleMultiTouch.touch(1,4000, 4000);
    bleMultiTouch.touch(2,4000, 6000);
    delay(50);
    bleMultiTouch.touch(1,6000, 4000);
    bleMultiTouch.touch(2,6000, 6000);
    bleMultiTouch.release(1);
    bleMultiTouch.release(2);
  }
  delay(2000);
}
