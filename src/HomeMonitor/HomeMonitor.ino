#include "Room.cpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Room* room1 = new Room();

  room1->addSensor(new Thermobeacon("Sensor1", "b5:70:00:00:06:c4"));
  room1->addSensor(new Thermobeacon("Sensor2", "b5:70:00:00:07:db"));

  Serial.println(room1->getTemperature(0));
  Serial.println(room1->getTemperature("Sensor2"));
  Serial.println(room1->getTemperature());

  Serial.println(room1->getHumidity(0));
  Serial.println(room1->getHumidity("Sensor2"));
  Serial.println(room1->getHumidity());

}

void loop()
{
  // put your main code here, to run repeatedly:
}