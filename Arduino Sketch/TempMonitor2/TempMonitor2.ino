#include "Nicla_System.h"
#include <ArduinoBLE.h>
#include "Arduino.h"
#include "Arduino_BHY2.h"

Sensor temp (SENSOR_ID_TEMP);



BLEService ServiceUUID("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEFloatCharacteristic CharacteristicUUID("19B10001-E8F2-537E-4F6C-D104768A1201" , BLERead | BLENotify);

float temperature;

void setup() {
  Serial.begin(115200);
    while (!Serial);

  if (!BLE.begin()) {
    Serial.println("ble failed");
    while(1);
  }


  nicla::begin();
  nicla::leds.begin();
  BLE.begin();
  BHY2.begin();
  temp.begin();

  BLE.setLocalName("TempMonitor");
  BLE.setAdvertisedService(ServiceUUID);

  ServiceUUID.addCharacteristic(CharacteristicUUID);

  CharacteristicUUID.writeValue(temp.value());

  BLE.addService(ServiceUUID);

  BLE.advertise();

  Serial.println("ble seaerching... ");

  nicla::leds.setColor(green);


}

void loop()
{
  BHY2.update();
  static auto time = millis();

  BLEDevice central = BLE.central();

  if (central)
  {
    Serial.println("Connected to Central:  ");
    Serial.println(central.address());

      while(central.connected())
      {
        static auto time = millis();
        BHY2.update();
        nicla::leds.setColor(blue);
        if(millis() - time >= 1000)
        {
          time = millis();

          nicla::leds.setColor(blue);
          temperature = temp.value();
          Serial.println(String(" Temp : ")  + String(temp.value()));

          CharacteristicUUID.writeValue(temp.value());
        }
      }

  }
  else{

    Serial.println(String("Disconnected"));
    nicla::leds.setColor(red);
  }
}





























