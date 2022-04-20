#include "House.cpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

WebServer server(80);

const char* ssid = "BTWholeHome-QZG";
const char* password = "DLvJdC6QhrQp";
String text = "Collecting Data.";  //Data to send to web page.

String thermobeaconDataJson = "";

TaskHandle_t Task1, Task2, MaintainTaskHandle;
static SemaphoreHandle_t mutex;

House* house1 = new House("House 1");

void setup() {
  Serial.begin(115200);

  initialiseWebServer();

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  house1->addRoom(new Room("Bedroom"));
  house1->addRoom(new Room("Kitchen"));
  house1->addRoom(new Room("Utility Room"));
  house1->addRoom(new Room("Garage"));

  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Window", "b5:70:00:00:06:c4"));
  house1->getRoom("Bedroom")->addControl(new ControlSystem("Heater", "temp", true));
  house1->getRoom("Bedroom")->addCycle("{\"type\": \"temp\",  \"goal\": 22,  \"startTime\": 1000,  \"endTime\": 2000,  \"activeDays\": \"mtwt-ss\",  \"active\": true}\"}");

  house1->getRoom("Kitchen")->addSensor(new Thermobeacon("Cupboard", "b5:70:00:00:07:db"));

  mutex = xSemaphoreCreateMutex();

  xTaskCreate(startBluetoothScan, "BluetoothTask", 70000, NULL, 1, &Task1);
  //  xTaskCreate(repeat, "repeatTask", 30000, NULL, 0, &Task2);
  xTaskCreate(startMaintainingState, "MaintainTask", 30000, NULL, 0, &MaintainTaskHandle);

  vTaskDelete(NULL); //End the Setup() and Loop() Tasks, as they are no longer needed.

}

void repeat(void * pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  vTaskDelay(30000 / portTICK_PERIOD_MS); //Wait 30 seconds before beginning processing, to collect useful data beforehand.

  while (true) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    xSemaphoreTake(mutex, portMAX_DELAY);
    if (thermobeaconDataJson != "") {
      Serial.println(house1->toJSON(thermobeaconDataJson));
    }
    xSemaphoreGive(mutex);
  }
}

void startMaintainingState(void * pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  vTaskDelay(20000 / portTICK_PERIOD_MS);
  Serial.println("Starting maintenance");
  while (true) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    house1->maintainHome(thermobeaconDataJson);
    xSemaphoreGive(mutex);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void stopMaintainingState() {
  vTaskDelete(MaintainTaskHandle);
}

void loop() {
  // put your main code here, to run repeatedly:

}
