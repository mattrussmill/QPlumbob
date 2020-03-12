/* BOARD: Arduino Nano 33 IoT */

#include <ArduinoBLE.h> //version 1.1.2
#include <FlashStorage.h> //version 0.7.1 github/cmaglie/FlashStorage
#include "U:/miller/Documents/MacoSpanks/deviceidentifiers.h" // must use absolute path arduino IDE

// nano 33 PWM duty cycle: 490 Hz (pins 5 and 6: 980 Hz)
#define PELVIS_PIN 9
#define GLUTEUS_PIN 10

// macros
#define TO_PWM(V) static_cast<int> round(2.55 * V)

// BLE Basic Authentication Service
BLEService authService(DevInfo::AUTH_SERVICE);

//BLE Basic Authentication Characteristics
BLEStringCharacteristic pinCharacteristic(DevInfo::PIN_CHARACTERISTIC, PIN_LENGTH, BLEWrite);
BLEBoolCharacteristic authCharacteristic(DevInfo::AUTH_STATUS_CHARACTERISTIC, BLERead | BLEIndicate); //TODO test notify vs indicate if value changed internally // and is it only on write? Do I need a separate value?
String pin;
bool authenticated = false;


// BLE Heating PWM Service
BLEService garmentService(DevInfo::GARMENT_SERVICE);

// BLE Garment Characteristics
BLEUnsignedCharCharacteristic pelvisCharacteristic(DevInfo::PELVIS_PWM_CHARACTERISTIC, BLERead | BLEWrite);
BLEUnsignedCharCharacteristic gluteusCharacteristic(DevInfo::GLUTEUS_PWM_CHARACTERISTIC, BLERead | BLEWrite);
BLEIntCharacteristic timerCharacteristic(DevInfo::TIMER_CHARACTERISTIC, BLERead | BLEWrite);
uint8_t pelvisValue = 0;
uint8_t gluteusValue = 0;
int timerStartValue = 0;


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

  // set initial pin to saved pin and authenticated to false
  pinCharacteristic.writeValue("000000"); //TODO This
  authCharacteristic.writeValue(false);

  // enable two regions for this device in the off position
  pelvisCharacteristic.writeValue(0);
  gluteusCharacteristic.writeValue(0);


  // setup BLE service
  authService.addCharacteristic(pinCharacteristic);
  authService.addCharacteristic(authCharacteristic);
  garmentService.addCharacteristic(timerCharacteristic);
  garmentService.addCharacteristic(pelvisCharacteristic);
  garmentService.addCharacteristic(gluteusCharacteristic);


  // setup BLE module
  BLE.addService(authService);
  BLE.addService(garmentService);
  BLE.setAdvertisedService(authService);        // Additional services will be discovered once connection is established.
  BLE.setDeviceName("MacoGarment_v0.0.1");      // device name -> how does this differ from garment?
  BLE.setLocalName("MacoGarment");              // advertising name
  //BLE.setAdvertisingInterval(320);            // 200 * 0.625 ms
  BLE.setConnectable(true);


  // set callback functions for events
  BLE.setEventHandler(BLEConnected, bleConnectedHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectedHandler);
  pinCharacteristic.setEventHandler(BLEWritten, pinWriteHandler); 
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

void pinWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  // steps to perform simple pin authentication
  if (pinCharacteristic.valueLength() == PIN_LENGTH)
  {
    uint8_t tryPin[PIN_LENGTH];
    pinCharacteristic.readValue(tryPin, PIN_LENGTH * sizeof(uint8_t));

    // if not authenticated try to authenticate
    if (!authenticated)
    {
      if (strcmp(tryPin, pin, PIN_LENGTH * sizeof(uint8_t)) == 0) //FIX
      {
        authCharacteristic.writeValue(true);
        authenticated = true;
        Serial.println("Device Authenticated!");
        return;
      }
      else // disconnect on fail
      {
        authCharacteristic.writeValue(false);
        BLE.disconnect();
      }
    }
    else //if authenticated, save pin
    {
      //TODO save pin
    }
  }
  authCharacteristic.writeValue(false);
  Serial.println("Authentication FAILED");
  BLE.disconnect();
  // disconnect on fail?
}

  
void timeoutWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  //TODO
}


void pelvisWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  if (authenticated)
    pelvisValue = pelvisCharacteristic.value(); //TODO when to place notify? Does it notify on write from central or write from internal?

  analogWrite(PELVIS_PIN, TO_PWM(pelvisValue));
  Serial.print("Pelvis Written: ");
  Serial.println(TO_PWM(pelvisCharacteristic.value()), DEC);
}


void gluteusWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  analogWrite(GLUTEUS_PIN, TO_PWM(gluteusCharacteristic.value()));
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
  authenticated = false;
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}
