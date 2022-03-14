#include "Thermobeacon.cpp"



void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Thermobeacon sensor1;
  sensor1.name = "Sensor1";
  sensor1.address = "b5:70:00:00:06:c4";
  Serial.println("Starting scan:");
  Serial.println(sensor1.getData());
  Serial.println("Scan complete!");
}

void loop()
{
  // put your main code here, to run repeatedly:
}
