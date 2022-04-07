#include "Arduino.h"
#include "ArduinoJson.h"

class Cycle {
  public:
    //Attributes
    String sourceJson; //The JSON from which this Cycle was defined, in the format: {type: String, target: Double, startTime: String (hh:mm), endTime: String (hh:mm), daysActive: short, active: Bool}.
    String variableType; //Temp, Hum, etc.
    double target; //The target value to maintain during the start and end times.
    String startTime; //The start time of the period in which to maintain the target value, in the format hh:mm.
    String endTime; //The end time of the period in which to maintain the target value, in the format hh:mm.
    byte daysActive; //A value between 1111111 and 0000000 (127 and 0) to represent which days of the week this cycle is active for. For Tuesday, Thursday, and Sunday, this would be 0101001 or 41.
    bool active; //A flag to determine if a cycle is active or not, for easy enabling/disabling.

    //Constructors and Destructors(including overloaded)
    Cycle() {};
    Cycle(String json) {
      sourceJson = json;
      constructFromSourceJson();
    }
    ~Cycle() {};
    
    //Concrete Methods
    void constructFromSourceJson(){
      if (sourceJson == ""){
        Serial.println("Error! sourceJson is empty!");
        return;
      }

      DynamicJsonDocument doc(512);
      DeserializationError e = deserializeJson(doc, sourceJson);
        if (e) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(e.c_str());
          return;
        } else {
          variableType = doc["type"].as<String>();
          target = doc["target"].as<double>();
          startTime = doc["startTime"].as<String>();
          endTime = doc["endTime"].as<String>();
          daysActive = doc["daysActive"].as<byte>();
          active = doc["active"].as<bool>();
        }      
    }


    
};
