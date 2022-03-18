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

    double getTemperature() {
      double tempSum = 0;
      int validTempCount = 0;
      for (int i = 0; i < numberOfSensors; i++) {
        String dataJSON = sensors[i]->getData();
        StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
        DeserializationError e = deserializeJson(doc, dataJSON);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["temp"] != -100.00) {
            tempSum += doc["temp"].as<double>();

            validTempCount++;
          }
        }
      }

      return (tempSum / validTempCount);
    }

    double getTemperature(String n) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          String dataJSON = sensors[i]->getData();
          StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
          DeserializationError e = deserializeJson(doc, dataJSON);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            if (doc["temp"] != -100.00) {
              return doc["temp"].as<double>();
            }
          }
        }
      }
      return -100.00;
    }

    double getTemperature(int index) {
      if (index <= numberOfSensors) {
        String dataJSON = sensors[index]->getData();
        StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
        DeserializationError e = deserializeJson(doc, dataJSON);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["temp"] != -100.00) {
            return doc["temp"].as<double>();
          }
        }
      }
      return -100.00;
    }

    double getHumidity() {
      double humSum = 0;
      int validHumCount = 0;
      for (int i = 0; i < numberOfSensors; i++) {
        String dataJSON = sensors[i]->getData();
        StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
        DeserializationError e = deserializeJson(doc, dataJSON);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["hum"] != -100.00) {
            humSum += doc["hum"].as<double>();
            validHumCount++;
          }
        }
      }
      return (humSum / validHumCount);
    }

    double getHumidity(String n) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          String dataJSON = sensors[i]->getData();
          StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
          DeserializationError e = deserializeJson(doc, dataJSON);
          if (e) {
            Serial.print("deserializeJson() failed with code ");
            Serial.println(e.c_str());
          } else {
            if (doc["hum"] != -100.00) {
              return doc["hum"].as<double>();
            }
          }
        }
      }
      return -100.00;
    }

    double getHumidity(int index) {
      if (index <= numberOfSensors) {
        String dataJSON = sensors[index]->getData();
        StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
        DeserializationError e = deserializeJson(doc, dataJSON);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
        } else {
          if (doc["hum"] != -100.00) {
            return doc["hum"].as<double>();
          }
        }
      }
      return -100.00;
    }

    String getData() {
      String dataJSON = "";

      double currentTemp = getTemperature();
      double currentHum = getHumidity();

      StaticJsonDocument<JSON_OBJECT_SIZE(6)> doc;
      doc["temp"] = currentTemp;
      doc["hum"] = currentHum;
      serializeJson(doc, dataJSON);

      return dataJSON;
    }

    String getData(String n) {
      for (int i = 0; i < numberOfSensors; i++) {
        if (sensors[i]->name == n) {
          return sensors[i]->getData();
        }
      }
    }

    String getData(int index) {
      if (index <= numberOfSensors){
        return sensors[index]->getData();
      }
    }


};
