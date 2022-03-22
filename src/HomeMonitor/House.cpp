#include "Arduino.h"
#include "Room.cpp"

#define BYTES_PER_HOUSE  64
#define BYTES_PER_ROOM  96
#define BYTES_PER_SENSOR  96
#define BYTES_BUFFER  100

#define MAX_ROOMS 50
#define MAX_SENSORS_PER_ROOM 50

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

    double getAverageTemperature(String n) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getTemperature();
        }
      }
      return -100.00;
    }

    double getAverageTemperature(int index) {
      if (index <= numberOfRooms) {
        return rooms[index]->getTemperature();
      }
      return -100.00;
    }


    double getAverageHumidity(String n) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getHumidity();
        }
      }
      return -100.00;
    }

    double getAverageHumidity(int index) {
      if (index <= numberOfRooms) {
        return rooms[index]->getHumidity();
      }
      return -100.00;
    }

    String getData(String n) {
      for (int i = 0; i < numberOfRooms; i++) {
        if (rooms[i]->name == n) {
          return rooms[i]->getData();
        }
      }
    }

    String getData(int index) {
      if (index <= numberOfRooms) {
        return rooms[index]->getData();
      }
    }

    String toJSON() {
      int numberOfSensors = 0;
      for (int i  = 0; i < numberOfRooms; i++) {
        numberOfSensors += rooms[i]->numberOfSensors;
      }

      DynamicJsonDocument doc(2048);

      doc["HouseName"] = name;
      doc["NumberOfRooms"] = numberOfRooms;

      JsonArray Rooms = doc.createNestedArray("Rooms");

      for (int i = 0; i < numberOfRooms; i++) {
        Room* currentRoom = rooms[i];
        JsonObject jsonRoom = Rooms.createNestedObject();
        jsonRoom["RoomName"] = currentRoom->name;
        jsonRoom["NumberOfSensors"] = currentRoom->numberOfSensors;
        jsonRoom["AverageTemperature"] = currentRoom->getTemperature();
        jsonRoom["AverageHumidity"] = currentRoom->getHumidity();

        JsonArray jsonSensorsInRoom = jsonRoom.createNestedArray("Sensors");

        for (int j = 0; j < currentRoom->numberOfSensors; j++) {
          JsonObject jsonSensor = jsonSensorsInRoom.createNestedObject();
          jsonSensor["SensorName"] = currentRoom->sensors[j]->name;
          jsonSensor["Temperature"] = currentRoom->getTemperature(j);
          jsonSensor["Humidity"] = currentRoom->getHumidity(j);
        }
      }

      String dataJSON = "";
      serializeJsonPretty(doc, dataJSON);
      return dataJSON;
    }


};
