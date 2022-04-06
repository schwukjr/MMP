#include "Arduino.h"
#include "Room.cpp"

#define BYTES_PER_HOUSE  64
#define BYTES_PER_ROOM  96
#define BYTES_PER_SENSOR  96
#define BYTES_BUFFER  100

#define MAX_ROOMS 10
#define MAX_SENSORS_PER_ROOM 10

class House {
  public:
    //Attributes
    String name;
    Room* rooms[MAX_ROOMS];
    int numberOfRooms = 0;

    //Constructors and Destructors(including overloaded)
    House() {};
    House(String n) {
      name = n;
    }
    ~House() {};
    //Concrete Methods
    void addRoom(Room* room) {
      if (numberOfRooms <= MAX_ROOMS) {
        rooms[numberOfRooms] = room;
        numberOfRooms++;
      } else {
        Serial.println("Room limit for house reached.");
      }
    }

    void removeRoom() {
      numberOfRooms--;
    }

    Room* getRoom(String n) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i];
        }
      }
      return new Room("null");
    }


    Room* getRoom(int index) {
      if (index <= numberOfRooms) {
        return rooms[index];
      }
      return new Room("null");
    }

    double getAverageTemperature(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getTemperature(thermobeaconDataJson);
        }
      }
      return -100.00;
    }

    double getAverageTemperature(int index, String thermobeaconDataJson) {
      if (index <= numberOfRooms) {
        return rooms[index]->getTemperature(thermobeaconDataJson);
      }
      return -100.00;
    }


    double getAverageHumidity(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getHumidity(thermobeaconDataJson);
        }
      }
      return -100.00;
    }

    double getAverageHumidity(int index, String thermobeaconDataJson) {
      if (index <= numberOfRooms) {
        return rooms[index]->getHumidity(thermobeaconDataJson);
      }
      return -100.00;
    }

    String getData(String n, String thermobeaconDataJson) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getData(thermobeaconDataJson);
        }
      }
    }

    String getData(int index, String thermobeaconDataJson) {
      if (index <= numberOfRooms) {
        return rooms[index]->getData(thermobeaconDataJson);
      }
    }

    String toJSON(String thermobeaconDataJson) {
      int numberOfSensors = 0;
      for (int i  = 0; i < numberOfRooms; i++) {
        numberOfSensors += rooms[i]->numberOfSensors;
      }

      DynamicJsonDocument doc(2048);

      doc["housename"] = name;
      doc["numberofrooms"] = numberOfRooms;

      JsonArray Rooms = doc.createNestedArray("rooms");

      for (int i = 0; i < numberOfRooms; i++) {
        Room* currentRoom = rooms[i];
        JsonObject jsonRoom = Rooms.createNestedObject();
        jsonRoom["roomname"] = currentRoom->name;
        jsonRoom["numberofsensors"] = currentRoom->numberOfSensors;
        jsonRoom["averagetemperature"] = currentRoom->getTemperature(thermobeaconDataJson);
        jsonRoom["averagehumidity"] = currentRoom->getHumidity(thermobeaconDataJson);

        JsonArray jsonSensorsInRoom = jsonRoom.createNestedArray("sensors");

        for (int j = 0; j < currentRoom->numberOfSensors; j++) {
          JsonObject jsonSensor = jsonSensorsInRoom.createNestedObject();
          jsonSensor["sensorname"] = currentRoom->sensors[j]->name;
          jsonSensor["temperature"] = currentRoom->getTemperature(j, thermobeaconDataJson);
          jsonSensor["humidity"] = currentRoom->getHumidity(j, thermobeaconDataJson);
        }
      }

      String dataJson = "";
      serializeJsonPretty(doc, dataJson);
      return dataJson;
    }


};
