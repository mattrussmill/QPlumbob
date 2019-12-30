/* BOARD: Arduino Nano 33 IoT */

#include <ArduinoBLE.h> //version 1.1.2

BLEService temperatureService("b1b01000-2a07-4cbb-80cb-1eaeaf18b124");  // BLE Heating Service
BLEShortCharacteristic regionDutyCycle("b1b01001-2a07-4cbb-80cb-1eaeaf18b124", BLERead | BLEWrite); // ms8b - Front / ls8b - Rear 0% to 100%
BLEDescriptor regionDutyCycleLabel("b1b01001-2a07-4cbb-80cb-1eaeaf18b124", "percent duty cycle");

void setup() {
  // initialize serial communication
  Serial.begin(9600);

  // initialize BLE module
  if (!BLE.begin())
  {
    Serial.println("BLE failed - device haulted");
    while (1);
  }
  
  // setup BLE characteristics
  regionDutyCycle.addDescriptor(regionDutyCycleLabel);
  regionDutyCycle.writeValue(0);            // set initial state to 0% duty cycle all regions
  
  // setup BLE service
  temperatureService.addCharacteristic(regionDutyCycle);

  // setup BLE module
  BLE.addService(temperatureService);
  BLE.setAdvertisedService(temperatureService); //@TODO is this necessary? Can I not advertise the service?
  BLE.setDeviceName("MacoGarment_v0.0.1");  // device name
  BLE.setLocalName("MacoGarment");          // advertising name
  BLE.setAdvertisingInterval(320);          // 200 * 0.625 ms
  BLE.setConnectable(true);

  BLE.advertise();                          //start advertising
  Serial.println("Initialization complete... advertising");
}

void loop() {
  // put your main code here, to run repeatedly:

}
