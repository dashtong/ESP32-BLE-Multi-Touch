#include "BleConnectionStatus.h"

BleConnectionStatus::BleConnectionStatus(void) {
}

void BleConnectionStatus::onConnect(BLEServer* server)
{
  this->connected = true;
  BLE2902* desc = (BLE2902*)this->inputMultiTouch->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
}

void BleConnectionStatus::onDisconnect(BLEServer* server)
{
  this->connected = false;
  BLE2902* desc = (BLE2902*)this->inputMultiTouch->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
  server->startAdvertising();
}
