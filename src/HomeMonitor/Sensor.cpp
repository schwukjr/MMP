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
    virtual String getData(String thermobeaconDataJson) {
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

  public:
    String address;

    Thermobeacon() {};
    Thermobeacon(String n, String a): Sensor(n) {
      address = a;
    }
    ~Thermobeacon() {};

    String getData(String thermobeaconDataJson)    {
      //Serial.println("Getting Thermobeacon Data.");

      StaticJsonDocument<JSON_OBJECT_SIZE(1500)> currentDataDoc;
      DeserializationError e = deserializeJson(currentDataDoc, thermobeaconDataJson);
      if (e) {
        Serial.print("deserializeJson() failed here with code ");
        Serial.println(e.c_str());
        return "!";
      }

      StaticJsonDocument<JSON_OBJECT_SIZE(15)> returnDataDoc;
      bool updated = false;

      for (int i = 0; i < sizeof(currentDataDoc["sensors"]); i++) {
        //Iterate through each current dataset, check if address of new matches, and update values.
        if (address == currentDataDoc["sensors"][i]["address"]) {
          returnDataDoc["address"] = address;
          returnDataDoc["temp"] = currentDataDoc["sensors"][i]["temp"];
          returnDataDoc["hum"] = currentDataDoc["sensors"][i]["hum"];
          returnDataDoc["volts"] = currentDataDoc["sensors"][i]["volts"];
          returnDataDoc["time"] = currentDataDoc["sensors"][i]["time"];
          updated = true;
          break; //Stop iterating as all other current datasets are not for this sensor.
        }
      }
      if (!updated) {
        returnDataDoc["address"] = address;
        returnDataDoc["temp"] = -100.00;
        returnDataDoc["hum"] = -100.00;
        returnDataDoc["volts"] = 0.00;
        returnDataDoc["time"] = "01/01/1900-00:00:00";
      }

      String returnValue = "";
      serializeJson(returnDataDoc, returnValue);
      //Serial.println("ReturnValue" + returnValue);
      return returnValue;

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

    String getData(String thermobeaconDataJson)
    {
      Serial.println("\nGetting OneWire Temp Data");

      OneWire oneWire(pinNo);
      DallasTemperature sensors(&oneWire);

      String dataJson = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      sensors.requestTemperatures();
      double celcius = sensors.getTempCByIndex(0);
      doc["temp"] = celcius;
      doc["hum"] = -100.00;
      serializeJson(doc, dataJson);
      return dataJson;
    }
};

class DummyHybridSensor : public Sensor
{
  private:

  public:
    DummyHybridSensor() {};
    DummyHybridSensor(String n): Sensor(n) {};
    ~DummyHybridSensor() {};

    String getData(String thermobeaconDataJson)
    {
      Serial.println("\nGetting Dummy Data");
      String dataJson = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = 25.2;
      doc["hum"] = 50.9;
      serializeJson(doc, dataJson);
      return dataJson;
    }
};

class DummyTemperatureSensor : public Sensor
{
  private:

  public:
    DummyTemperatureSensor() {};
    DummyTemperatureSensor(String n): Sensor(n) {};
    ~DummyTemperatureSensor() {};

    String getData(String thermobeaconDataJson)
    {
      Serial.println("\nGetting Dummy Data");
      String dataJson = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = 25.2;
      doc["hum"] = '!';
      serializeJson(doc, dataJson);
      return dataJson;
    }
};

class DummyHumiditySensor : public Sensor
{
  private:

  public:
    DummyHumiditySensor() {};
    DummyHumiditySensor(String n): Sensor(n) {};
    ~DummyHumiditySensor() {};

    String getData(String thermobeaconDataJson)
    {
      Serial.println("\nGetting Dummy Data");
      String dataJson = "";
      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = '!';
      doc["hum"] = 50.9;
      serializeJson(doc, dataJson);
      return dataJson;
    }
};
