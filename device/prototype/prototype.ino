/* BOARD: Arduino Nano 33 IoT - SAMD21 Cortex M0 - default in little endian mode*/

#include <ArduinoBLE.h> //version 1.1.2
#include <FlashStorage.h> //version 1.0.0 github/cmaglie/FlashStorage
#include "C:/Users/mattr/Documents/QPlumbob/deviceidentifiers.h" // must use absolute path arduino IDE

// nano 33 PWM duty cycle: 490 Hz (pins 5 and 6: 980 Hz)
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11
#define RESET_PIN 2

// defaults
#define DEFAULT_PIN "0000"

// macros
#define TO_PWM(V) static_cast<int> round(2.55 * V)

// BLE Basic Authentication Service
BLEService authService(DevInfo::AUTH_SERVICE);
BLEStringCharacteristic pinCharacteristic(DevInfo::PIN_CHARACTERISTIC, PIN_LENGTH, BLEWrite);
BLEBoolCharacteristic authCharacteristic(DevInfo::AUTH_STATUS_CHARACTERISTIC, BLERead | BLENotify);
String pin;
bool authenticated = false;

// BLE LED Service
BLEService ledService(DevInfo::LED_SERVICE);
BLEUnsignedCharCharacteristic hueCharacteristic(DevInfo::HUE_CHARACTERISTIC, BLERead | BLEWrite);
BLEUnsignedCharCharacteristic saturationCharacteristic(DevInfo::SATURATION_CHARACTERISTIC, BLERead | BLEWrite);
uint8_t hueHsvValue = 0;
uint8_t saturationValue = 0;

// FlashStorage
FlashStorage(savedPin, String);

void setup()
{
  // initialize serial communication
  Serial.begin(9600);


  // initialize I/O
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(RED_PIN, OUTPUT);
  analogWrite(RED_PIN, 255);

  pinMode(GREEN_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 255);

  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(BLUE_PIN, 255);

  pinMode(RESET_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RESET_PIN), resetNonvolatileMemory, RISING);


  // initialize BLE module
  if (!BLE.begin())
  {
    Serial.println("BLE failed - device haulted");
    while (1);
  }


  /* setup BLE characteristics */

  // set initial pin to saved pin and authenticated to false
  pin = savedPin.read();
  authCharacteristic.writeValue(false);
  Serial.println("THIS IS THE INITIAL PIN: " + pin);
  if (pin == "") //set default if empty
  {
    Serial.println("Pin initialized to default");
    pin = DEFAULT_PIN;
    savedPin.write(pin);
  }
  pinCharacteristic.setEventHandler(BLEWritten, pinWriteHandler);
  

  // enable two regions for this device in the off position
  hueCharacteristic.writeValue(hueHsvValue);
  saturationCharacteristic.writeValue(saturationValue);

  // setup BLE service
  authService.addCharacteristic(pinCharacteristic);
  authService.addCharacteristic(authCharacteristic);
  ledService.addCharacteristic(hueCharacteristic);
  ledService.addCharacteristic(saturationCharacteristic);


  // setup BLE module
  BLE.addService(authService);
  BLE.addService(ledService);
  BLE.setAdvertisedService(authService);        // Additional services will be discovered once connection is established.
  BLE.setDeviceName("Plumbob_v0.0.1");
  BLE.setLocalName("Plumbob");                  // advertising name
  //BLE.setAdvertisingInterval(320);            // 200 * 0.625 ms
  BLE.setConnectable(true);


  // set callback functions for events
  BLE.setEventHandler(BLEConnected, bleConnectedHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectedHandler);
  hueCharacteristic.setEventHandler(BLEWritten, hueWriteHandler);
  saturationCharacteristic.setEventHandler(BLEWritten, saturationWriteHandler);

  // finish initialization
  BLE.advertise();                              // start advertising
  Serial.println("Initialization complete... advertising");

  // enable interrupts
  interrupts();
}


void loop()
{
  // put your main code here, to run repeatedly:
  BLE.poll();
}

void pinWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  // steps to perform simple pin authentication
  if (!authenticated && pinCharacteristic.value() == pin)
  {
    authCharacteristic.writeValue(true);
    authenticated = true;
    Serial.println("Device Authenticated!");
    return;
  }
  else if (authenticated)
  {
    // set new pin by writing the pin to non-volatile memory
    pin = pinCharacteristic.value();
    savedPin.write(pin);
    Serial.println("New pin saved!");
    return;
  }

  Serial.println("Authentication FAILED");
  authCharacteristic.writeValue(false);
  disconnectFromCentralActions();
  BLE.disconnect();
}

void hueWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  if (!isAuthenticated()) return;
  hueHsvValue = hueCharacteristic.value();
  setRgbValues();
  Serial.print("Hue Written: ");
  Serial.println(hueHsvValue, DEC);
}

void saturationWriteHandler(BLEDevice central, BLECharacteristic characteristic)
{
  if (!isAuthenticated()) return;
  saturationValue = saturationCharacteristic.value();
  setRgbValues();
  Serial.print("Saturation Written: ");
  Serial.println(saturationValue, DEC);
}

void setRgbValues()
{
  // adapted from http://dystopiancode.blogspot.com/2012/06/hsv-rgb-conversion-algorithms-in-c.html
  float c = 0.0, m = 0.0, x = 0.0, h = 0.0;
  h = hueHsvValue * 2.0; // set to 360 / 2 = 180 to fit in uint8 -> revert from central device
  c = saturationValue / 255.0; //v * s; v = 1.0, saturation normalized to 0.0 - 1.0
  x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
  m = 1.0 - c; // v - c;
  if (h >= 0.0 && h < 60.0)
  {
    setRgbPwmOutput(c + m, x + m, m);
  }
  else if (h >= 60.0 && h < 120.0)
  {
    setRgbPwmOutput(x + m, c + m, m);
  }
  else if (h >= 120.0 && h < 180.0)
  {
    setRgbPwmOutput(m, c + m, x + m);
  }
  else if (h >= 180.0 && h < 240.0)
  {
    setRgbPwmOutput(m, x + m, c + m);
  }
  else if (h >= 240.0 && h < 300.0)
  {
    setRgbPwmOutput(x + m, m, c + m);
  }
  else if (h >= 300.0 && h < 360.0)
  {
    setRgbPwmOutput(c + m, m, x + m);
  }
  else
  {
    setRgbPwmOutput(m, m, m);
  }
}

void setRgbPwmOutput(const float red, const float green, const float blue)
{
  // r,g,b normalized at 0.0 - 1.0
  analogWrite(RED_PIN, TO_PWM(red * 255));
  analogWrite(GREEN_PIN, TO_PWM(green * 255));
  analogWrite(BLUE_PIN, TO_PWM(blue * 255));
}

void bleConnectedHandler(BLEDevice central)
{
  // central connected event handler
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Connected event, central: ");
  Serial.println(central.address());

  // TODO timeout if not connected long enough
}


void bleDisconnectedHandler(BLEDevice central)
{
  // central disconnected event handler
  Serial.println("DISCONNECT HANDLER CALLED");
  disconnectFromCentralActions();
}

// actions to take on disconnect - disconnect handler not called if LED performs disconnect
void disconnectFromCentralActions()
{
  authCharacteristic.writeValue(false);
  authenticated = false;
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected event, central: ");
  Serial.println(BLE.central().address());
}

bool isAuthenticated()
{
  if (!authenticated)
  {
    Serial.println("Authentication FAILED");
    disconnectFromCentralActions();
    BLE.disconnect();
  }
  return authenticated;
}

void resetNonvolatileMemory()
{
  //button debounced with cap externally
  unsigned long startCount = 0;
  while (digitalRead(RESET_PIN) == HIGH)
  {
    startCount++;
    // reset at ~6 seconds (rough estimate)
    if (startCount > 5000000) {
      savedPin.write(DEFAULT_PIN);
      Serial.println("DEVICE RESTORED TO DEFAULT - PLEASE RESTART DEVICE!");
      break;
    }
  }
}
