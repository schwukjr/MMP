#include "Arduino.h"

class Sensor {
  public:
    //Attributes
    String name;

    //Constructors and Destructors(including overloaded)
    Sensor() {};
    Sensor(String n) {
      name = n;
    }
    ~Sensor() {};
    //Concrete Methods

    //Abstract Methods
    virtual String getData() {
      return "";
    };



};

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include "ArduinoJson.h"

class Thermobeacon : public Sensor
{
  private:
    double value_from_hex_data(const char *service_data, int offset, int data_length, bool reverse, bool canBeNegative = true)
    {
      char data[data_length + 1];
      memcpy(data, &service_data[offset], data_length);
      data[data_length] = '\0';
      long value;
      if (reverse)
      {
        // reverse data order
        char rev_data[data_length + 1];
        revert_hex_data(data, rev_data, data_length + 1);
        value = strtol(rev_data, NULL, 16);
      }
      else
      {
        value = strtol(data, NULL, 16);
      }
      if (value > 65000 && data_length <= 4 && canBeNegative)
        value = value - 65535;
      // Log.trace(F("value %D" CR), value);
      return value;
    }

    void revert_hex_data(const char *in, char *out, int l)
    {
      // reverting array 2 by 2 to get the data in good order
      int i = l - 2, j = 0;
      while (i != -2)
      {
        if (i % 2 == 0)
          out[j] = in[i + 1];
        else
          out[j] = in[i - 1];
        j++;
        i--;
      }
      out[l - 1] = '\0';
    }

    String process_ws02(char *manufacturerdata, String address)
    {
      String dataJSON = "";

      double currentTemp = value_from_hex_data(manufacturerdata, 24, 4, true) / 16;
      double currentHum = value_from_hex_data(manufacturerdata, 28, 4, true) / 16;

      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = currentTemp;
      doc["hum"] = currentHum;
      serializeJson(doc, dataJSON);

      //      Serial.print("Temperature:");
      //      Serial.println(currentTemp);
      //      Serial.print("Humidity:");
      //      Serial.println(currentHum);
      //      Serial.print("Voltage:");
      //      Serial.println(value_from_hex_data(manufacturerdata, 20, 4, true) / 1000);

      return dataJSON;
    }


  public:
    String address;

    Thermobeacon() {};
    Thermobeacon(String n, String a): Sensor(n) {
      address = a;
    }
    ~Thermobeacon() {};

    String getData()
    {
      Serial.println("\nGetting Thermobeacon Data");
      BLEScan *pBLEScan;

      BLEDevice::init("");
      pBLEScan = BLEDevice::getScan(); // create new scan
      pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster
      pBLEScan->setInterval(100);
      pBLEScan->setWindow(99); // less or equal setInterval value

      while (true) {
        BLEScanResults foundDevices = pBLEScan->start(5, false);
        if (foundDevices.getCount() > 0) {
          for (int i = 0; i < foundDevices.getCount(); i++) {
            String addr(foundDevices.getDevice(i).getAddress().toString().c_str());

            if (addr == address) {
              std::string strManufacturerData = foundDevices.getDevice(i).getManufacturerData();
              char cManufacturerData[100];
              strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

              if (strManufacturerData.length() == 20 && cManufacturerData[0] == 0x10 && cManufacturerData[1] == 0x00)
              {
                char* manufacturerdata = BLEUtils::buildHexData(NULL, (uint8_t*)foundDevices.getDevice(i).getManufacturerData().data(), foundDevices.getDevice(i).getManufacturerData().length());
                String result = process_ws02(manufacturerdata, addr);
                free(manufacturerdata);
                pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
                //free(pBLEScan);
                Serial.println("Successful!");
                return result;
              }
            }
          }
        }
      }


    }
};

#include <OneWire.h>
#include <DallasTemperature.h>

class OneWireTempSensor : public Sensor
{
  private:

  public:
    int pinNo;

    OneWireTempSensor() {};
    OneWireTempSensor(String n, int i): Sensor(n) {
      pinNo = i;
    };
    ~OneWireTempSensor() {};

    String getData()
    {
      Serial.println("\nGetting OneWire Temp Data");

      OneWire oneWire(pinNo);
      DallasTemperature sensors(&oneWire);

      String dataJSON = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      sensors.requestTemperatures();
      double celcius = sensors.getTempCByIndex(0);
      doc["temp"] = celcius;
      doc["hum"] = -100.00;
      serializeJson(doc, dataJSON);
      return dataJSON;
    }
};

class DummyHybridSensor : public Sensor
{
  private:

  public:
    DummyHybridSensor() {};
    DummyHybridSensor(String n): Sensor(n) {};
    ~DummyHybridSensor() {};

    String getData()
    {
      Serial.println("\nGetting Dummy Data");
      String dataJSON = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = 25.2;
      doc["hum"] = 50.9;
      serializeJson(doc, dataJSON);
      return dataJSON;
    }
};

class DummyTemperatureSensor : public Sensor
{
  private:

  public:
    DummyTemperatureSensor() {};
    DummyTemperatureSensor(String n): Sensor(n) {};
    ~DummyTemperatureSensor() {};

    String getData()
    {
      Serial.println("\nGetting Dummy Data");
      String dataJSON = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = 25.2;
      doc["hum"] = '!';
      serializeJson(doc, dataJSON);
      return dataJSON;
    }
};

class DummyHumiditySensor : public Sensor
{
  private:

  public:
    DummyHumiditySensor() {};
    DummyHumiditySensor(String n): Sensor(n) {};
    ~DummyHumiditySensor() {};

    String getData()
    {
      Serial.println("\nGetting Dummy Data");
      String dataJSON = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = '!';
      doc["hum"] = 50.9;
      serializeJson(doc, dataJSON);
      return dataJSON;
    }
};
