#include "Arduino.h"

class ControlSystem {
  public:
    //Attributes
    String name; //An identifier for this control system, i.e. "Heater" or "Humidifier"/"Dehumidifier".
    String variableType; //This will match to the type specified in a "Cycle", i.e. "temp" or "hum".
    bool additiveSystem; //Represents whether this system will raise a variable to an above normal level (i.e. Heater or Humidifier) or lower said variable to a below normal level (AC/Fan or Dehumidifier).
    bool enabled = false; //Represents whether this system is currently active or not. Default to false as it has not been enabled yet.
    String type; //This refers to the type of ControlSystem this is, such as "Relay".

    //Constructors and Destructors(including overloaded)
    ControlSystem() {};
    ControlSystem(String n, String vT, bool aS, String t) {
      name = n;
      variableType = vT;
      additiveSystem = aS;
      type = t;
    }
    ~ControlSystem() {};
    //Concrete Methods

    //Methods
    virtual void enable() {
      if (!enabled) {
        enabled = true;
        Serial.println(name + " enabled.");
      }
    };

    virtual void disable() {
      if (enabled) {
        enabled = false;
        Serial.println(name + " disabled.");
      }
    };


};


class Relay : public ControlSystem {
  public:
    //Attributes
    int pin; //This refers to the pin on the ESP-32 that the relay is connected to.
    bool activeHigh; //This refers to whether the relay is active when a HIGH signal is applied.

    //Constructors and Destructors(including overloaded)
    Relay() {};
    Relay(String n, String vT, bool aS, String t, int p, bool aH) : ControlSystem(n, vT, aS, t) {
      pin = p;
      activeHigh = aH;
      
      pinMode(pin, OUTPUT);
      if (activeHigh){ //Default relay to inactive.
        digitalWrite(pin, LOW);
      } else {
        digitalWrite(pin, HIGH);
      }
    }
    ~Relay() {};
    //Concrete Methods

    //Methods
    void enable() {
      ControlSystem::enable();
      if (activeHigh){ //Default relay to inactive.
        digitalWrite(pin, HIGH);
      } else {
        digitalWrite(pin, LOW);
      }
    };

    void disable() {
      ControlSystem::disable();
      if (activeHigh){ //Default relay to inactive.
        digitalWrite(pin, LOW);
      } else {
        digitalWrite(pin, HIGH);
      }
    };


};
