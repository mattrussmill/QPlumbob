#ifndef DEVICEIDENTIFIERS_H
#define DEVICEIDENTIFIERS_H

namespace DevInfo
{

// BLE Device Services (Version 4 UUID)
const char GARMENT_SERVICE[]            = "3e777786-787c-40f1-bf9d-9f73b73f583a";   // BLE Functionality of Garment
const char AUTH_SERVICE[]               = "a8ca8262-1407-4fc3-8230-382ad0c93b7a";   // Basic Pin Authentication
const char DEVICE_INFO_SERVICE[]        = "180A";                                   // TODO setup official GATT service

// BLE Garment Service Characteristics (Version 4 UUID)
const char TIMER_CHARACTERISTIC[]       = "6c167447-315a-4ea2-ae31-f7753a115689";   // BLE Safety Shutoff Service
const char PELVIS_PWM_CHARACTERISTIC[]  = "7d424b36-ed7a-480f-bb0c-da80d4d84c2f";   // BLE Temperature Control for Pelvis Region
const char GLUTEUS_PWM_CHARACTERISTIC[] = "9dcba0ce-ad99-46a5-b9f0-57141f1226ee";   // BLE Temperature Control for Gluteus Region
//5bbabe83-34a2-4e50-abe6-58031ce5603e
//50438927-a6f5-42bc-85cb-2fe98f6a137c

// BLE Basic Authentication Characteristics (Version 4 UUID)
const char PIN_CHARACTERISTIC[]         = "94ed26a1-f1d0-4730-90e6-fe360d3ab67c";   // BLE Pin Stored for Connecting to Device
const char AUTH_STATUS_CHARACTERISTIC[] = "9fece778-2ac2-47d4-8527-3ea1fca68acc";   // BLE Status if Write Access Works

// BLE Device Information Service Characteristics
const char DEVICE_INFO_CHAR[]       = {'\0'};                                       // TODO setup official GATT characteristics
}

#endif // DEVICEIDENTIFIERS_H
