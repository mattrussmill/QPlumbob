#ifndef DEVICEIDENTIFIERS_H
#define DEVICEIDENTIFIERS_H
#define PIN_LENGTH 6

namespace DevInfo
{

// BLE Device Services (Version 4 UUID)
const char LED_SERVICE[]            = "3e777786-787c-40f1-bf9d-9f73b73f583a";   // BLE Functionality of LED
const char AUTH_SERVICE[]           = "a8ca8262-1407-4fc3-8230-382ad0c93b7a";   // Basic Pin Authentication
const char DEVICE_INFO_SERVICE[]    = "180A";                                   // TODO setup official GATT service

// BLE LED Service Characteristics (Version 4 UUID)
const char HUE_CHARACTERISTIC[]         = "6c167447-315a-4ea2-ae31-f7753a115689";   // BLE For displayed color 0-359 deg normalized to 0-255 - hue in hsv model
const char SATURATION_CHARACTERISTIC[]  = "7d424b36-ed7a-480f-bb0c-da80d4d84c2f";   // BLE For saturation 0-100% - saturation in hsv model
const char VALUE_CHARACTERISTIC[]       = "a25c19cc-573e-4391-893e-a995c97936e6";   // BLE For lamp intensity 0-100% - value in hsv model
const char PERIOD_CHARACTERISTIC[]      = "9dcba0ce-ad99-46a5-b9f0-57141f1226ee";   // BLE For rate at which it takes make one hue cycle in ms

// BLE Basic Authentication Characteristics (Version 4 UUID)
const char PIN_CHARACTERISTIC[]         = "94ed26a1-f1d0-4730-90e6-fe360d3ab67c";   // BLE Pin Stored for Connecting to Device
const char AUTH_STATUS_CHARACTERISTIC[] = "9fece778-2ac2-47d4-8527-3ea1fca68acc";   // BLE Status for Authentication Updates
const char AUTH_NOTIFY_DESCRIPTOR[]     = "2902";                                   // BLE Descriptor to enable/disable pushed authentication updates
                                                                                        // ^ BLE standard UUID is equal to QBluetoothUuid::ClientCharacteristicConfiguration

// BLE Device Information Service Characteristics
const char DEVICE_INFO_CHAR[]       = {'\0'};                                       // TODO setup official GATT characteristics
}

#endif // DEVICEIDENTIFIERS_H
