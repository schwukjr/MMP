#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include "ArduinoJson.h"

void setup() {

}

double value_from_hex_data(const char* service_data, int offset, int data_length, bool reverse, bool canBeNegative = true) {
  char data[data_length + 1];
  memcpy(data, &service_data[offset], data_length);
  data[data_length] = '\0';
  long value;
  if (reverse) {
    // reverse data order
    char rev_data[data_length + 1];
    revert_hex_data(data, rev_data, data_length + 1);
    value = strtol(rev_data, NULL, 16);
  } else {
    value = strtol(data, NULL, 16);
  }
  if (value > 65000 && data_length <= 4 && canBeNegative)
    value = value - 65535;
  //Log.trace(F("value %D" CR), value);
  return value;
}

void revert_hex_data(const char* in, char* out, int l) {
  //reverting array 2 by 2 to get the data in good order
  int i = l - 2, j = 0;
  while (i != -2) {
    if (i % 2 == 0)
      out[j] = in[i + 1];
    else
      out[j] = in[i - 1];
    j++;
    i--;
  }
  out[l - 1] = '\0';
}

void process_ws02( char* manufacturerdata, String address) {
  double temp = value_from_hex_data(manufacturerdata , 24, 4, true) / 16;
  double hum = value_from_hex_data(manufacturerdata , 28, 4, true) / 16;
  double voltage = value_from_hex_data(manufacturerdata , 20, 4, true) / 1000;

  StaticJsonDocument<9> doc;

  doc["address"] = address;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["volts"] = voltage;

  //Limit responses to realistic data. (Thermobeacons have max operating temp. of 65 Degrees C)
  Serial.print("Temperature:");
  Serial.println(temp);
  Serial.print("Humidity:");
  Serial.println(hum);
  Serial.print("Voltage:");
  Serial.println(value_from_hex_data(manufacturerdata , 20, 4, true) / 1000);
}

void startBluetoothScan() {
  Serial.println("\nGetting Thermobeacon Data");
  BLEScan *pBLEScan;

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // create new scan
  pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value

  DynamicJsonDocument(2048) doc;
  JsonArray Sensors = doc.createNestedArray("sensors");

  while (true) {
    BLEScanResults foundDevices = pBLEScan->start(5, false);
    if (foundDevices.getCount() > 0) {
      for (int i = 0; i < foundDevices.getCount(); i++) {
        String addr(foundDevices.getDevice(i).getAddress().toString().c_str());

        std::string strManufacturerData = foundDevices.getDevice(i).getManufacturerData();
        char cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

        if (strManufacturerData.length() == 20 && cManufacturerData[0] == 0x10 && cManufacturerData[1] == 0x00)
        {
          char* manufacturerdata = BLEUtils::buildHexData(NULL, (uint8_t*)foundDevices.getDevice(i).getManufacturerData().data(), foundDevices.getDevice(i).getManufacturerData().length());
          String result = process_ws02(manufacturerdata, addr);

          StaticJsonDocument<JSON_OBJECT_SIZE(6)> sensorDataDoc;
          DeserializationError e = deserializeJson(sensorDataDoc, dataJSON);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            JsonObject newSensor = Sensors.createNestedObject();
            newSensor["address"] = sensorDataDoc["address"];
            newSensor["temp"] = sensorDataDoc["temp"];
            newSensor["hum"] = sensorDataDoc["hum"];
            newSensor["volts"] = sensorDataDoc["volts"];
          }

          free(manufacturerdata);
          pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
          Serial.println("Data collected from: " + sensorDataDoc["address"]);
        }
      }
      serializeJsonPretty(doc. Serial);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}


void loop() {

}
