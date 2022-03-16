#include "Sensor.cpp"


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Sensor* sensors[50];
  int numberOfSensors = 0;

  sensors[numberOfSensors] = new Thermobeacon("Sensor1", "b5:70:00:00:06:c4");
  numberOfSensors++;
  sensors[numberOfSensors] = new DummySensor("sensor2");
  numberOfSensors++;

  for (int i = 0; i < numberOfSensors; i++) {
    Serial.println(sensors[i]->name + ": " + sensors[i]->getData());
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
}
