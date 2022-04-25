#include "Arduino.h"
#include "Room.cpp"

#define BYTES_PER_HOUSE  64
#define BYTES_PER_ROOM  96
#define BYTES_PER_SENSOR  96
#define BYTES_BUFFER  100

#define MAX_ROOMS 10
#define MAX_SENSORS_PER_ROOM 10
#define MAX_CYCLES_PER_ROOM 5
#define MAX_CONTROLS_PER_ROOM 5

class House {
  private:
    bool cycleActive(Cycle* cycle) {
      int currentDayAsNumber = currentDayOfWeekAsNumber();
      bool activeToday = (cycle->activeDays[currentDayAsNumber] != '-');
      int currentTime = currentTimeAsNumber();
      bool activeNow = (currentTime >= cycle->startTime && currentTime <= cycle->endTime);
      return activeToday && activeNow && cycle->active;
      //A cycle is active if the current day of the week is within the active days of the cycle, the current time is between the start and end times, and the active flag is true.
    }

    int currentDayOfWeekAsNumber() { //Returns the current day of the week (as per the NTP server) as an integer between 1 and 7 inclusive.
      struct tm timeinfo;
      char currentDayOfWeek[1] = "";
      while (true) {
        if (getLocalTime(&timeinfo)) {
          strftime(currentDayOfWeek, 2, "%w", &timeinfo);
          return atoi(currentDayOfWeek);
        } else {
          vTaskDelay(200 / portTICK_PERIOD_MS);
        }
      }
    }

    int currentTimeAsNumber() {
      struct tm timeinfo;
      char currentTime[4] = "";
      while (true) {
        if (getLocalTime(&timeinfo)) {
          strftime(currentTime, 5, "%H%M", &timeinfo);
          return atoi(currentTime);
        } else {
          vTaskDelay(200 / portTICK_PERIOD_MS);
        }
      }
    }

  public:
    //Attributes
    String name;
    Room* rooms[MAX_ROOMS];
    int numberOfRooms = 0;
    bool maintainingState = true;

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
          jsonSensor["name"] = currentRoom->sensors[j]->name;
          jsonSensor["temperature"] = currentRoom->getTemperature(j, thermobeaconDataJson);
          jsonSensor["humidity"] = currentRoom->getHumidity(j, thermobeaconDataJson);
        }

        JsonArray jsonCyclesInRoom = jsonRoom.createNestedArray("cycles");

        for (int j = 0; j < currentRoom->numberOfCycles; j++) {
          JsonObject jsonCycle = jsonCyclesInRoom.createNestedObject();
          jsonCycle["type"] = currentRoom->cycles[j]->type;
          jsonCycle["goal"] = currentRoom->cycles[j]->goal;
          jsonCycle["starttime"] = currentRoom->cycles[j]->startTime;
          jsonCycle["endtime"] = currentRoom->cycles[j]->endTime;
          jsonCycle["activedays"] = currentRoom->cycles[j]->activeDays;
          jsonCycle["active"] = currentRoom->cycles[j]->active;
        }

        JsonArray jsonControlsInRoom = jsonRoom.createNestedArray("controls");

        for (int j = 0; j < currentRoom->numberOfControls; j++) {
          JsonObject jsonControl = jsonControlsInRoom.createNestedObject();
          jsonControl["name"] = currentRoom->controls[j]->name;
          jsonControl["type"] = currentRoom->controls[j]->type;
          jsonControl["additivesystem"] = currentRoom->controls[j]->additiveSystem;
          jsonControl["enabled"] = currentRoom->controls[j]->enabled;
        }
      }

      String dataJson = "";
      serializeJsonPretty(doc, dataJson);
      return dataJson;
    }

    //ToDo update function to deserialise control systems and cycles.
    void constructWithJson(String inputJson) {
      DynamicJsonDocument doc(2048);
      DeserializationError e = deserializeJson(doc, inputJson);
      if (e) {
        Serial.print("deserializeJson() failed with code ");
        Serial.println(e.c_str());
      } else {
        name = doc["name"].as<String>();
        numberOfRooms = doc["numberofrooms"].as<int>();

        for (int i = 0; i < numberOfRooms; i++) {
          String roomName = doc["rooms"][i]["roomname"].as<String>();
          int numberOfSensors = doc["rooms"][i]["numberofsensors"].as<int>();

          Room* newRoom = new Room(roomName);

          for (int j = 0; j < numberOfSensors; j++) {
            String sensorType = doc["rooms"][i]["sensors"][j]["sensortype"].as<String>();
            String sensorName = doc["rooms"][i]["sensors"][j]["sensorname"].as<String>();

            if (sensorType == "Thermobeacon") {
              String macAddr = doc["rooms"][i]["sensors"][j]["macaddress"].as<String>();
              newRoom->addSensor(new Thermobeacon(sensorName, macAddr));
            } else if (sensorType == "OneWire") {
              int pin = doc["rooms"][i]["sensors"][j]["pinnumber"].as<int>();
              newRoom->addSensor(new OneWireTempSensor(sensorName, pin));
            } else if (sensorType == "DummyHybrid") {
              newRoom->addSensor(new DummyHybridSensor(sensorName));
            } else if (sensorType == "DummyTemp") {
              newRoom->addSensor(new DummyTemperatureSensor(sensorName));
            } else if (sensorType == "DummyHum") {
              newRoom->addSensor(new DummyHumiditySensor(sensorName));
            } else {
              Serial.println("Sensor not found!");
            }
          }

          addRoom(newRoom);
        }
      }
    }

    void maintainHome(String thermobeaconDataJson) {
      for (int roomIndex = 0; roomIndex < numberOfRooms; roomIndex++) {
        //Iterate through each room
        Room* room = rooms[roomIndex];
        Serial.println("Current Room: " + room->name);

        for (int cycleIndex = 0; cycleIndex < room->numberOfCycles; cycleIndex++) {
          //For each room, iterate through each cycle
          Cycle* currentCycle = room->cycles[cycleIndex];
          Serial.println("Current Cycle: " + currentCycle->sourceJson);

          if (cycleActive(currentCycle)) {
            //Determine if cycle is active.
            Serial.println("Cycle active.");

            for (int controlIndex = 0; controlIndex < room->numberOfControls; controlIndex++) {
              //For each active cycle, iterate through each control to determine a match of "type".
              ControlSystem* currentControl = room->controls[controlIndex];
              Serial.println("Current Control: " + currentControl->name);

              if (currentControl->type == currentCycle->type) {
                double currentVariableLevel = -100;
                if (currentCycle->type == "temp") {
                  currentVariableLevel = room->getTemperature(thermobeaconDataJson);
                  Serial.println("Temp: " + String(currentVariableLevel));
                } else if (currentCycle->type == "hum") {
                  currentVariableLevel = room->getHumidity(thermobeaconDataJson);
                  Serial.println("Hum: " + String(currentVariableLevel));
                }

                if (currentControl->additiveSystem == true && (currentVariableLevel < currentCycle->goal - 1)) {
                  currentControl->enable();
                } else if (currentControl->additiveSystem == true && (currentVariableLevel > currentCycle->goal + 1)) {
                  currentControl->disable();

                }

                if (currentControl->additiveSystem == false && (currentVariableLevel > currentCycle->goal + 1)) {
                  currentControl->enable();
                } else if (currentControl->additiveSystem == false && (currentVariableLevel < currentCycle->goal - 1)) {
                  currentControl->disable();
                }



              }
              Serial.println("Control Enabled: " + String(currentControl->enabled) + "\n");
            }

          }
        }



        //Once an active cycle is matched to a control system, use average room temperature/humidity to determine if the level needs to be raised or lowered, and if an appropriate system is available (based on "additiveSystem" flag).
        //If current level is more than a set amount above/below goal, activate appropriate system.
        //If current level is within a set amount of goal, it can be left alone, and control system deactivated if not already. Move onto next cycle.
        //Continue iterating over each cycle indefinitely, using matching control system to raise, lower, or leave level alone.
        //Pause briefly to allow other processes to take place, namely collection of up-to-date sensor data.

      }
    }

};
