#include "House.cpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  House* house1 = new House("House1");

  house1->addRoom(new Room("Bedroom"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor1", "b5:70:00:00:06:c4"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor2", "b5:70:00:00:07:db"));
  house1->addRoom(new Room("Bathroom"));
  house1->getRoom("Bathroom")->addSensor(new OneWireTempSensor("Sensor3", 5));
  Serial.println("Average: " + String(house1->getAverageTemperature("Bedroom"), 2));
  Serial.println("Average: " + String(house1->getAverageTemperature("Bathroom"), 2));

  //  Room* room1 = new Room();
  //
  //  room1->addSensor(new Thermobeacon("Sensor1", "b5:70:00:00:06:c4"));
  //  room1->addSensor(new Thermobeacon("Sensor2", "b5:70:00:00:07:db"));
  //
  //  Serial.println(room1->getTemperature(0));
  //  Serial.println(room1->getTemperature("Sensor2"));
  //  Serial.println(room1->getTemperature());
  //
  //  Serial.println(room1->getHumidity(0));
  //  Serial.println(room1->getHumidity("Sensor2"));
  //  Serial.println(room1->getHumidity());

}

void loop()
{
  // put your main code here, to run repeatedly:
}
