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

BLEService serviceUUID(BLE_UUID("0000"));
BLEFloatCharacteristic tempUUID(BLE_UUID("1001"), BLERead | BLENotify);
BLEFloatCharacteristic pressureUUID(BLE_UUID("2001"), BLERead | BLENotify);
BLEFloatCharacteristic humidityUUID(BLE_UUID("3001"), BLERead | BLENotify);
BLEFloatCharacteristic gasUUID(BLE_UUID("4001"), BLERead | BLENotify);
BLEFloatCharacteristic iaqUUID(BLE_UUID("5001"), BLERead | BLENotify);
BLEFloatCharacteristic co2UUID(BLE_UUID("6001"), BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("ble failed");
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

  BLE.setLocalName("ValueMonitor");
  BLE.setAdvertisedService(serviceUUID);

  serviceUUID.addCharacteristic(tempUUID);
  serviceUUID.addCharacteristic(pressureUUID);
  serviceUUID.addCharacteristic(humidityUUID);
  serviceUUID.addCharacteristic(gasUUID);
  serviceUUID.addCharacteristic(iaqUUID);
  serviceUUID.addCharacteristic(co2UUID);

  BLE.addService(serviceUUID);

  BLE.advertise();

  Serial.println("ble searching...");
}

void loop() {
  static unsigned long time = millis();
  static unsigned long gastime = millis();

  if(BLE.connected()){
    Serial.println("Connected to Central: ");
    BLE.stopAdvertise();
  }
  while (BLE.connected()) {

    nicla::leds.setColor(blue);
    // If you want to print the central address, ensure you have the necessary code for it.

    BHY2.update();
    nicla::leds.setColor(blue);

    if (millis() - gastime >= 11000) {
      gastime = millis();
      gasUUID.writeValue(gas.value());
      iaqUUID.writeValue(static_cast<float>(bsec.iaq()));
      co2UUID.writeValue(bsec.co2_eq());
    }

    if (millis() - time >= 2500) {
      time = millis();
      tempUUID.writeValue(temp.value());
      pressureUUID.writeValue(baro.value());
      humidityUUID.writeValue(humidity.value());
    }
  }

  if (BLE.disconnect()) {
    Serial.println("Disconnected");
    BLE.advertise();
    nicla::leds.setColor(red);
  }
}
