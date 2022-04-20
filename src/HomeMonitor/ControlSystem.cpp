#include "Arduino.h"

class ControlSystem {
  public:
    //Attributes
    String name; //An identifier for this control system, i.e. "Heater" or "Humidifier"/"Dehumidifier".
    String type; //This will match to the type specified in a "Cycle", i.e. "temp" or "hum".
    bool increasing; //Represents whether this system will raise a variable to an above normal level (i.e. Heater or Humidifier) or lower said variable to a below normal level (AC/Fan or Dehumidifier).
    bool enabled = false; //Represents whether this system is currently active or not. Default to false as it has not been enabled yet.

    //Constructors and Destructors(including overloaded)
    ControlSystem() {};
    ControlSystem(String n, String t, bool i) {
      name = n;
      type = t;
      increasing = i;
    }
    ~ControlSystem() {};
    //Concrete Methods

    //Methods
    void enable() {
      if (!enabled) {
        enabled = true;
        Serial.println(name + " enabled.");
      }
    };

    void disable() {
      if (enabled) {
        enabled = false;
        Serial.println(name + " disabled.");
      }
    };


};
