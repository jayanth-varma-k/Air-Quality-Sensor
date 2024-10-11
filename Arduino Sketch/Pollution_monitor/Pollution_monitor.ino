#include "Nicla_System.h"
#include <ArduinoBLE.h>
#include "Arduino_BHY2.h"
#include "Arduino.h"

#define BLE_UUID(val) ("19b10000-" val "-537e-4f6c-d104768a1214")

Sensor temp(SENSOR_ID_TEMP);
Sensor baro(SENSOR_ID_BARO);
Sensor humidity(SENSOR_ID_HUM);
Sensor gas(SENSOR_ID_GAS);
SensorBSEC bsec(SENSOR_ID_BSEC);
SensorXYZ acc(SENSOR_ID_ACC);

BLEService serviceUUID(BLE_UUID("0000"));
BLEFloatCharacteristic tempUUID(BLE_UUID("1001"), BLERead | BLENotify);
BLEFloatCharacteristic pressureUUID(BLE_UUID("2001"), BLERead | BLENotify);
BLEFloatCharacteristic humidityUUID(BLE_UUID("3001"), BLERead | BLENotify);
BLEFloatCharacteristic gasUUID(BLE_UUID("4001"), BLERead | BLENotify);
BLEFloatCharacteristic iaqUUID(BLE_UUID("5001"), BLERead | BLENotify);
BLEFloatCharacteristic co2UUID(BLE_UUID("6001"), BLERead | BLENotify);
//BLEFloatCharacteristic accUUID(BLE_UUID("7001"), BLERead | BLENotify);
BLEFloatCharacteristic flagUUID(BLE_UUID("8001"), BLERead | BLENotify);

unsigned long previousMillis = 0;
const long interval = 1500;
float flag = 0;
float minacc = 5000;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("BLE failed");
    while (1);
  }

  nicla::begin();
  nicla::leds.begin();

  BHY2.begin();

  temp.begin();
  baro.begin();
  humidity.begin();
  gas.begin();
  bsec.begin();
  acc.begin();

  BLE.setLocalName("ValueMonitor");
  BLE.setAdvertisedService(serviceUUID);

  serviceUUID.addCharacteristic(tempUUID);
  serviceUUID.addCharacteristic(pressureUUID);
  serviceUUID.addCharacteristic(humidityUUID);
  serviceUUID.addCharacteristic(gasUUID);
  serviceUUID.addCharacteristic(iaqUUID);
  serviceUUID.addCharacteristic(co2UUID);
  //serviceUUID.addCharacteristic(accUUID);
  serviceUUID.addCharacteristic(flagUUID);

  BLE.addService(serviceUUID);

  BLE.advertise();

  Serial.println("BLE searching...");

  nicla::leds.setColor(red);
}

void loop() {
  static unsigned long time = millis();

  if (BLE.connected()) {
    Serial.println("Connected to Central: ");
    BLE.stopAdvertise();
    nicla::leds.setColor(blue);
  }

  while (BLE.connected()) {
    BHY2.update();
    unsigned long currentMillis = millis();

      float x, y, z;
      x = abs(acc.x());
      y = abs(acc.y());
      z = abs(acc.z());
      
      /*
      Serial.println("Acceleration ");
      Serial.print("X: ");
      Serial.println(x);
      Serial.print("y: ");
      Serial.println(y);  
      Serial.print("z: ");
      Serial.println(z);
      Serial.print("Flag: ");
      Serial.print(flag);
      */

      if (x < minacc && y < minacc && z < minacc) {
        flag = 1;
      } else {
        flag = 0;
      }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Read sensor values
      tempUUID.writeValue(temp.value());
      pressureUUID.writeValue(baro.value());
      humidityUUID.writeValue(humidity.value());
      gasUUID.writeValue(gas.value());
      iaqUUID.writeValue(static_cast<float>(bsec.iaq()));
      co2UUID.writeValue(bsec.co2_eq());
      flagUUID.writeValue(flag);
      Serial.println("Values Sent");

      Serial.println(temp.value());
      Serial.println(baro.value());
      Serial.println(humidity.value());
      Serial.println(gas.value());
      Serial.println(static_cast<float>(bsec.iaq()));
      Serial.println(bsec.co2_eq());
      Serial.println(flag);
    }
  }

  if (!BLE.connected()) {
    Serial.println("Disconnected");
    BLE.advertise();
    nicla::leds.setColor(red);
  }

  while (!BLE.connected()) {
    BHY2.update();
      /*float x, y, z;
      x = abs(acc.x());
      y = abs(acc.y());
      z = abs(acc.z());
      
      Serial.println("Acceleration ");
      Serial.print("X: ");
      Serial.println(x);
      Serial.print("y: ");
      Serial.println(y);
      Serial.print("z: ");
      Serial.println(z);
      Serial.print("Flag: ");
      Serial.println(flag);

      if (x < minacc && y < minacc && z < minacc) {
        flag = 1;
      } else {
        flag = 0;
      }
      */
  }
}
