#include "Arduino.h"
#include "ArduinoJson.h"

class Cycle {
  public:
    //Attributes
    String sourceJson; //The JSON from which this Cycle was defined, in the format: {type: String, goal: Double, startTime: String (hh:mm), endTime: String (hh:mm), activeDays: String, active: Bool}.
    String type; //"temp" or "hum", more types can be added later on (such as "atm" for atmospheric pressure or "light" for light levels.
    double goal; //The target value to maintain during the start and end times.
    int startTime; //The start time of the period in which to maintain the target value, in the format hhmm (military time).
    int endTime; //The end time of the period in which to maintain the target value, in the format hhmm (military time).
    String activeDays; //A string containing the first letter of days in the sunday-starting week this cycle will be active for
    //(or "-" for inactive days), i.e. "SMT-T-S" for Sunday/Monday/Tuesday/Thursday/Saturday. Matches the format used by strftime.
    bool active; //A flag to determine if a cycle is active or not, for easy enabling/disabling.

    //Constructors and Destructors(including overloaded)
    Cycle() {};
    Cycle(String json) {
      sourceJson = json;
      constructFromSourceJson();
    }
    Cycle(String t, double g, int st, int et, String ad, bool a) {
      type = t;
      goal = g;
      startTime = st;
      endTime = et;
      activeDays = ad;
      active = a;
    }
    ~Cycle() {};

    //Concrete Methods
    void constructFromSourceJson() {
      if (sourceJson == "") {
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
        type = doc["type"].as<String>();
        goal = doc["goal"].as<double>();
        startTime = doc["startTime"].as<int>();
        endTime = doc["endTime"].as<int>();
        activeDays = doc["activeDays"].as<String>();
        active = doc["active"].as<bool>();
      }
    }



};
