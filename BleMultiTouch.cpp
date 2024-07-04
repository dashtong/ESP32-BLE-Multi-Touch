#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "BleMultiTouch.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
  #include "esp32-hal-log.h"
  #define LOG_TAG ""
#else
  #include "esp_log.h"
  static const char* LOG_TAG = "BLEDevice";
#endif

#define REPORTID_TOUCH        0x01

#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)

static const uint8_t _hidReportDescriptor[] = {
  0x05, 0x0D,                    // USAGE_PAGE(Digitizers)
  0x09, 0x04,                    // USAGE     (Touch Screen)
  0xA1, 0x01,                    // COLLECTION(Application)
  0x85, REPORTID_TOUCH,          //   REPORT_ID (Touch)

  // define the maximum amount of fingers that the device supports
  0x09, 0x55,                    //   USAGE (Contact Count Maximum)
  0x25, 0x05,   //   LOGICAL_MAXIMUM (CONTACT_COUNT_MAXIMUM)
  0xB1, 0x02,                    //   FEATURE (Data,Var,Abs)

  // define the actual amount of fingers that are concurrently touching the screen
  0x09, 0x54,                    //   USAGE (Contact count)
  0x95, 0x01,                    //   REPORT_COUNT(1)
  0x75, 0x08,                    //   REPORT_SIZE (8)
  0x81, 0x02,                    //   INPUT (Data,Var,Abs)

  // declare a finger collection
  0x09, 0x22,                    //   USAGE (Finger)
  0xA1, 0x02,                    //   COLLECTION (Logical)

  // declare an identifier for the finger
  0x09, 0x51,                    //     USAGE (Contact Identifier)
  0x75, 0x08,                    //     REPORT_SIZE (8)
  0x95, 0x01,                    //     REPORT_COUNT (1)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)

  // declare Tip Switch and In Range
  0x09, 0x42,                    //     USAGE (Tip Switch)
  0x09, 0x32,                    //     USAGE (In Range)
  0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
  0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,                    //     REPORT_SIZE (1)
  0x95, 0x02,                    //     REPORT_COUNT(2)
  0x81, 0x02,                    //     INPUT (Data,Var,Abs)

  // declare the remaining 6 bits of the first data byte as constant -> the driver will ignore them
  0x95, 0x06,                    //     REPORT_COUNT (6)
  0x81, 0x03,                    //     INPUT (Cnst,Ary,Abs)

  // define absolute X and Y coordinates of 16 bit each (percent values multiplied with 100)
  0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,                    //     Usage (X)
  0x09, 0x31,                    //     Usage (Y)
  0x16, 0x00, 0x00,              //     Logical Minimum (0)
  0x26, 0x10, 0x27,              //     Logical Maximum (10000)
  0x36, 0x00, 0x00,              //     Physical Minimum (0)
  0x46, 0x10, 0x27,              //     Physical Maximum (10000)
  0x66, 0x00, 0x00,              //     UNIT (None)
  0x75, 0x10,                    //     Report Size (16),
  0x95, 0x02,                    //     Report Count (2),
  0x81, 0x02,                    //     Input (Data,Var,Abs)
  0xC0,                          //   END_COLLECTION
  0xC0                           // END_COLLECTION

  // with this declaration a data packet must be sent as:
  // byte 1   -> "contact count"        (always == 1)
  // byte 2   -> "contact identifier"   (any value)
  // byte 3   -> "Tip Switch" state     (bit 0 = Tip Switch up/down, bit 1 = In Range)
  // byte 4,5 -> absolute X coordinate  (0...10000)
  // byte 6,7 -> absolute Y coordinate  (0...10000)
};


BleMultiTouch::BleMultiTouch(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) : hid(0)
{
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
  this->connectionStatus = new BleConnectionStatus();
}

void BleMultiTouch::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleMultiTouch::end(void)
{
}

bool BleMultiTouch::isConnected(void)
{
  return this->connectionStatus->connected;
}

void BleMultiTouch::setBatteryLevel(uint8_t level)
{
  this->batteryLevel = level;
  if (hid != 0)
    this->hid->setBatteryLevel(this->batteryLevel);
}

void BleMultiTouch::send(uint8_t identifier, int state, int16_t x, int16_t y)
{
  if (this->isConnected())
  {
    uint8_t data[] = {
      1,           
      identifier,        // contact identifier
      state,             // finger touches display
      MSB(x), LSB(x),    // x
      MSB(y), LSB(y)     // y
    };
    this->inputMultiTouch->setValue(data, 7);
    this->inputMultiTouch->notify();
  }
}

void BleMultiTouch::touch(uint8_t identifier, int16_t x, int16_t y)
{
  this->send(identifier, 1, x, y);
}

void BleMultiTouch::release(uint8_t identifier) 
{
  this->send(identifier, 0, 0, 0);
}

void BleMultiTouch::tap(uint8_t identifier, int16_t x, int16_t y)
{
  this->touch(identifier, x, y);
  this->release(identifier);
}

void BleMultiTouch::taskServer(void *pvParameter)
{
  BleMultiTouch *bleTouchInstance = (BleMultiTouch *)pvParameter; //static_cast<BleMouse *>(pvParameter);
  BLEDevice::init(bleTouchInstance->deviceName);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(bleTouchInstance->connectionStatus);

  bleTouchInstance->hid = new BLEHIDDevice(server);
  bleTouchInstance->inputMultiTouch = bleTouchInstance->hid->inputReport(REPORTID_TOUCH); // <-- input REPORTID from report map
  bleTouchInstance->connectionStatus->inputMultiTouch = bleTouchInstance->inputMultiTouch;

  bleTouchInstance->hid->manufacturer()->setValue(bleTouchInstance->deviceManufacturer);

  bleTouchInstance->hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  bleTouchInstance->hid->hidInfo(0x00, 0x02);

  BLESecurity *security = new BLESecurity();

  security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  bleTouchInstance->hid->reportMap((uint8_t *)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  bleTouchInstance->hid->startServices();

  bleTouchInstance->onStarted(server);

  BLEAdvertising *advertising = server->getAdvertising();
  advertising->setAppearance(HID_MOUSE);
  advertising->addServiceUUID(bleTouchInstance->hid->hidService()->getUUID());
  advertising->start();
  bleTouchInstance->hid->setBatteryLevel(bleTouchInstance->batteryLevel);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY);
}
