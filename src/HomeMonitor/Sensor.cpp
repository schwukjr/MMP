#include "Arduino.h"

class Sensor{
  public:
    //Attributes
    String name;

    //Constructors and Destructors(including overloaded)
    Sensor(){};
    ~Sensor(){};
    //Concrete Methods

    //Abstract Methods
    virtual String getData(){};



};
