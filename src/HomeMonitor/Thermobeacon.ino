#include "time.h"

void startBluetoothScan(void * pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  vTaskDelay(2000 / portTICK_PERIOD_MS);

  Serial.println("\nGetting Thermobeacon Data");
  BLEScan *pBLEScan;

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // create new scan
  pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value

  while (true) {
    DynamicJsonDocument doc(2048);
    JsonArray Sensors = doc.createNestedArray("sensors");

    Serial.println("\nStarting scan!");
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

          StaticJsonDocument<JSON_OBJECT_SIZE(15)> sensorDataDoc;
          DeserializationError e = deserializeJson(sensorDataDoc, result);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            JsonObject newSensor = Sensors.createNestedObject();
            newSensor["address"] = addr;
            newSensor["temp"] = sensorDataDoc["temp"].as<double>();
            newSensor["hum"] = sensorDataDoc["hum"].as<double>();
            newSensor["volts"] = sensorDataDoc["volts"].as<double>();
            newSensor["time"] = sensorDataDoc["time"];
          }

          free(manufacturerdata);
          pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
          Serial.println("Data collected from: " + sensorDataDoc["address"].as<String>());
//          Serial.println("Count: " + String(foundDevices.getCount()));
        }
      }


      
      String jsonBuffer = "";
      serializeJsonPretty(doc, jsonBuffer);
      updateThermobeaconDataJson(jsonBuffer);
      Serial.println();
    }
    Serial.println("Scan complete!");
    //Serial.println(thermobeaconDataJson);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void updateThermobeaconDataJson(String newData) {
  String currentData = String(thermobeaconDataJson);

  if (currentData == "") {
    //No data has been saved to "current" dataset, so all "new" data can be saved.
    thermobeaconDataJson = String(newData);
    return;
  }

    DynamicJsonDocument newDataDoc(2048);
  DeserializationError e = deserializeJson(newDataDoc, newData);
  if (e) {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(e.c_str());
    return;
  }

    DynamicJsonDocument currentDataDoc(2048);
  e = deserializeJson(currentDataDoc, currentData);
  if (e) {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(e.c_str());
    return;
  }

  for (int i = 0; i < sizeof(newDataDoc["sensors"]); i++) {
    //Iterate through each new dataset.
    JsonObject newData = newDataDoc["sensors"][i];
    bool updated = false;
    for (int j = 0; j < sizeof(currentDataDoc["sensors"]); j++) {
      //Iterate through each current dataset, check if address of new matches, and update values.
      if (newData["address"] == currentDataDoc["sensors"][j]["address"]) {
        currentDataDoc["sensors"][j]["temp"] = newData["temp"];
        currentDataDoc["sensors"][j]["hum"] = newData["hum"];
        currentDataDoc["sensors"][j]["volts"] = newData["volts"];
        currentDataDoc["sensors"][j]["time"] = newData["time"];
        updated = true;
        break; //Stop iterating as all other current datasets are not for this sensor.
      }
    }
    if (!updated) {
      JsonObject sensor = currentDataDoc["sensors"].createNestedObject();
      sensor["address"] = newData["address"];
      sensor["temp"] = newData["temp"];
      sensor["hum"] = newData["hum"];
      sensor["volts"] = newData["volts"];
      sensor["time"] = newData["time"];
    }
  }

  String updatedData = "";
  serializeJsonPretty(currentDataDoc, updatedData);
  xSemaphoreTake(mutex, portMAX_DELAY);
  thermobeaconDataJson = String(updatedData);
  Serial.println("Running for: " + String(millis() / 1000) + "s");
  xSemaphoreGive(mutex);

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

String process_ws02( char* manufacturerdata, String address) {
  double temp = value_from_hex_data(manufacturerdata , 24, 4, true) / 16;
  double hum = value_from_hex_data(manufacturerdata , 28, 4, true) / 16;
  double voltage = value_from_hex_data(manufacturerdata , 20, 4, true) / 1000;

  char currentTimeStamp[20] = "";

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    String("01/01/1900-00:00:00").toCharArray(currentTimeStamp, 20);
  }
  strftime(currentTimeStamp, 20, "%m/%d/%y-%H:%M:%S", &timeinfo);

  StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;

  doc["address"] = address;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["volts"] = voltage;
  doc["time"] = currentTimeStamp;

  //  Serial.print("Temperature:");
  //  Serial.println(temp);
  //  Serial.print("Humidity:");
  //  Serial.println(hum);
  //  Serial.print("Voltage:");
  //  Serial.println(value_from_hex_data(manufacturerdata , 20, 4, true) / 1000);

  String returnValue = "";
  serializeJson(doc, returnValue);
  return returnValue;
}
