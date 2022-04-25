#include "House.cpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

AsyncWebServer server(80);

const char* ssid = "BTWholeHome-QZG";
const char* password = "DLvJdC6QhrQp";
char text[5000];  //Data to send to web page.

String thermobeaconDataJson = "";

TaskHandle_t BluetoothTaskHandle, OutputTaskHandle, MaintainTaskHandle;
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

  xTaskCreate(startBluetoothScan, "BluetoothTask", 30000, NULL, 1, &BluetoothTaskHandle);
  xTaskCreate(generateWebPageDataOutput, "OutputTask", 50000, NULL, 0, &OutputTaskHandle);
  xTaskCreate(startMaintainingState, "MaintainTask", 30000, NULL, 0, &MaintainTaskHandle);

  vTaskDelete(NULL); //End the Setup() and Loop() Tasks, as they are no longer needed.

}

void repeat(void * pvParameters) {
  Serial.print("OutputTaskHandle running on core ");
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
  Serial.print("MaintainTask running on core ");
  Serial.println(xPortGetCoreID());

  vTaskDelay(10000 / portTICK_PERIOD_MS); //Wait 30 seconds before beginning processing, to collect useful data beforehand.

  Serial.println("Starting maintenance");
  while (true) {
    if (house1->maintainingState) {
      xSemaphoreTake(mutex, portMAX_DELAY);
      house1->maintainHome(thermobeaconDataJson);
      xSemaphoreGive(mutex);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
void stopMaintainingState() {
  vTaskDelete(MaintainTaskHandle);
}

void generateWebPageDataOutput(void * pvParameters) {
  vTaskDelay(30000 / portTICK_PERIOD_MS); //Wait 30 seconds before beginning processing, to collect useful data beforehand.
  Serial.print("OutputTaskHandle running on core ");
  Serial.println(xPortGetCoreID());
  while (true) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    String houseJson = house1->toJSON(thermobeaconDataJson);
    xSemaphoreGive(mutex);
    houseJson.toCharArray(text, houseJson.length()); 
    Serial.println(text);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}


void loop() {
  // put your main code here, to run repeatedly:

}
