#ifndef ESP32_BLE_MULTI_TOUCH_H
#define ESP32_BLE_MULTI_TOUCH_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BleConnectionStatus.h"
#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

class BleMultiTouch {
private:
  BleConnectionStatus* connectionStatus;
  BLEHIDDevice* hid;
  BLECharacteristic* inputMultiTouch;
  static void taskServer(void* pvParameter);
public:
  BleMultiTouch(std::string deviceName = "ESP32 Bluetooth Multi Touch", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void);
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  void send(uint8_t identifier, int state, int16_t x, int16_t y);
  void touch(uint8_t identifier, int16_t x, int16_t y);
  void release(uint8_t identifier);
  void tap(uint8_t identifier, int16_t x, int16_t y);

  uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;
protected:
  virtual void onStarted(BLEServer *pServer) { };
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_MULTI_TOUCH_H
