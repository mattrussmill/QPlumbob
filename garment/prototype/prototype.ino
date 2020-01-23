/* BOARD: Arduino Nano 33 IoT */

#include <ArduinoBLE.h> //version 1.1.2
#include "U:/miller/Documents/MacoSpanks/deviceidentifiers.h" // must use absolute path arduino IDE

// nano 33 PWM duty cycle: 490 Hz (pins 5 and 6: 980 Hz)
#define PELVIS_PIN 10
#define GLUTEUS_PIN 11

// macros
#define TO_PWM(V) static_cast<unsigned char> round(2.55 * V)

// BLE Heating PWM Service
BLEService garmentService(DevInfo::GARMENT_SERVICE);

// BLE Garment Characteristics : 4 header bytes, 2 data bytes for heat element regions available for pwm data format
BLEUnsignedCharCharacteristic pelvisCharacteristic(DevInfo::PELVIS_PWM_CHARACTERISTIC, BLERead | BLEWrite);
BLEUnsignedCharCharacteristic gluteusCharacteristic(DevInfo::GLUTEUS_PWM_CHARACTERISTIC, BLERead | BLEWrite);
BLEIntCharacteristic timerCharacteristic(DevInfo::TIMER_CHARACTERISTIC, BLERead | BLEWrite);


void setup()
{
  // initialize serial communication
  Serial.begin(9600);


  // initialize I/O
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(PELVIS_PIN, OUTPUT);
  analogWrite(PELVIS_PIN, 0);

  pinMode(GLUTEUS_PIN, OUTPUT);
  analogWrite(GLUTEUS_PIN, 0);


  // initialize BLE module
  if (!BLE.begin())
  {
    Serial.println("BLE failed - device haulted");
    while (1);
  }


  /* setup BLE characteristics */
  // set initial state to -1; timer disabled
  timerCharacteristic.writeValue(-1);


  // enable two regions for this device in the off position
  pelvisCharacteristic.writeValue(0);
  gluteusCharacteristic.writeValue(0);


  // setup BLE service
  garmentService.addCharacteristic(timerCharacteristic);
  garmentService.addCharacteristic(pelvisCharacteristic);
  garmentService.addCharacteristic(gluteusCharacteristic);


  // setup BLE module
  BLE.addService(garmentService);
  BLE.setAdvertisedService(garmentService);     // @TODO is this necessary? Can I not advertise the service?
  BLE.setDeviceName("MacoGarment_v0.0.1");      // device name -> how does this differ from garment?
  BLE.setLocalName("MacoGarment");              // advertising name
  //BLE.setAdvertisingInterval(320);            // 200 * 0.625 ms
  BLE.setConnectable(true);


  // set callback functions for events
  BLE.setEventHandler(BLEConnected, bleConnectedHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectedHandler);
  pelvisCharacteristic.setEventHandler(BLEWritten, pelvisWriteHandler);
  gluteusCharacteristic.setEventHandler(BLEWritten, gluteusWriteHandler);

  // finish initialization
  BLE.advertise();                              // start advertising
  Serial.println("Initialization complete... advertising");
}


void loop()
{
  // put your main code here, to run repeatedly:
  BLE.poll();

}


void timeoutWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  //TODO
}


void pelvisWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  Serial.print("Pelvis Written: ");
  Serial.println(TO_PWM(pelvisCharacteristic.value()), DEC);
}


void gluteusWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  Serial.print("Gluteus Written: ");
  Serial.println(TO_PWM(gluteusCharacteristic.value()), DEC);
}


void bleConnectedHandler(BLEDevice central)
{
  // central connected event handler
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}


void bleDisconnectedHandler(BLEDevice central)
{
  // central disconnected event handler
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}
