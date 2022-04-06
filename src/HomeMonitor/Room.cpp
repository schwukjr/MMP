#include "Arduino.h"
#include "Sensor.cpp"

#define MAX_SENSORS 50

class Room {
  public:
    //Attributes
    String name;
    Sensor* sensors[MAX_SENSORS];
    int numberOfSensors = 0;

    //Constructors and Destructors(including overloaded)
    Room() {};
    Room(String n) {
      name = n;
    }
    ~Room() {};
    //Concrete Methods
    void addSensor(Sensor* sensor) {
      if (numberOfSensors <= MAX_SENSORS) {
        sensors[numberOfSensors] = sensor;
        numberOfSensors++;
      } else {
        Serial.println("Sensor limit for room reached.");
      }
    }

    void removeSensor() {
      numberOfSensors--;
    }

    double getTemperature(String thermobeaconDataJson) {
      double tempSum = 0;
      int validTempCount = 0;
      for (int i = 0; i < numberOfSensors; i++) {
        String dataJson = sensors[i]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
        DeserializationError e = deserializeJson(doc, dataJson);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["temp"].as<double>() != -100.00) {
            tempSum += doc["temp"].as<double>();
            validTempCount++;
          }
        }
      }

      return (tempSum / validTempCount);
    }

    double getTemperature(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          String dataJson = sensors[i]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
          DeserializationError e = deserializeJson(doc, thermobeaconDataJson);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            return doc["temp"].as<double>();
          }
        }
      }
      return -100.00;
    }

    double getTemperature(int index, String thermobeaconDataJson) {
      if (index <= numberOfSensors) {
        String dataJson = sensors[index]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
        DeserializationError e = deserializeJson(doc, dataJson);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          return doc["temp"].as<double>();
        }
      }
      return -100.00;
    }


    double getHumidity(String thermobeaconDataJson) {
      double humSum = 0;
      int validHumCount = 0;
      for (int i = 0; i < numberOfSensors; i++) {
        String dataJson = sensors[i]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
        DeserializationError e = deserializeJson(doc, dataJson);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["hum"].as<double>() != -100.00) {
            humSum += doc["hum"].as<double>();
            validHumCount++;
          }
        }
      }
      return (humSum / validHumCount);
    }

    double getHumidity(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          String dataJson = sensors[i]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
          DeserializationError e = deserializeJson(doc, dataJson);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            return doc["hum"].as<double>();
          }
        }
      }
      return -100.00;
    }

    double getHumidity(int index, String thermobeaconDataJson) {
      if (index <= numberOfSensors) {
        String dataJson = sensors[index]->getData(thermobeaconDataJson);
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
        DeserializationError e = deserializeJson(doc, dataJson);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          return doc["hum"].as<double>();
        }
      }
      return -100.00;
    }

    String getData(String thermobeaconDataJson) {
      double tempSum = 0;
      int validTempCount = 0;
      double humSum = 0;
      int validHumCount = 0;

      for (int i = 0; i < numberOfSensors; i++) {
        String dataJson = sensors[i]->getData(thermobeaconDataJson);
        StaticJsonDocument<JSON_OBJECT_SIZE(1500)> doc;
        DeserializationError e = deserializeJson(doc, dataJson);
        if (e) {
          Serial.print("deserializeJson() failed there with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["temp"].as<double>() != -100.00) {
            tempSum += doc["temp"].as<double>();
            validTempCount++;
          }
          if (doc["hum"].as<double>() != -100.00) {
            humSum += doc["hum"].as<double>();
            validHumCount++;
          }
        }
      }
      
      StaticJsonDocument<JSON_OBJECT_SIZE(15)> doc;
      doc["temp"] = tempSum / validTempCount;
      doc["hum"] = humSum / validHumCount;

      String returnJSON = "";
      serializeJson(doc, returnJSON);
      return returnJSON;
    }

    String getData(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          return sensors[i]->getData(thermobeaconDataJson);
        }
      }
    }

    String getData(int index, String thermobeaconDataJson) {
      if (index <= numberOfSensors) {
        return sensors[index]->getData(thermobeaconDataJson);
      }
    }


};
