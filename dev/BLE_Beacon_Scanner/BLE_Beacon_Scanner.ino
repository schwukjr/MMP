/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <ArduinoJson.h>

String text = "";
double temp;
double hum;

StaticJsonDocument<JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(9)> doc;

const String page PROGMEM = "<head>"
                            " <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
                            " </head>"
                            " <body>"
                            " <h1>Thermobeacon Sensor Data: </h1><h1 id=\"add1\">""<h1><h2>Temperature:</h2> <h2 id=\"temp1\">""</h2>\r\n"
                            " <h2>Humidity:</h2><h2 id=\"hum1\">""</h2>\r\n"
                            " <hr>\r\n"
                            " <h1>Thermobeacon Sensor Data: </h1><h1 id=\"add2\">""<h1><h2>Temperature:</h2> <h2 id=\"temp2\">""</h2>\r\n"
                            " <h2>Humidity:</h2><h2 id=\"hum2\">""</h2>\r\n"
                            " <script>\r\n"
                            " $(document).ready(function(){\r\n"
                            " setInterval(getData,1000);\r\n"
                            " function getData(){\r\n"
                            " $.ajax({\r\n"
                            "  type:\"GET\",\r\n"
                            "  url:\"data\",\r\n"
                            "  success: function(data){\r\n"
                            "  let s = JSON.parse(data);\r\n"
                            "  $('#add1').html(s[0].address);\r\n"
                            "  $('#temp1').html(s[0].temp);\r\n"
                            "  $('#hum1').html(s[0].hum);\r\n"
                            "  $('#add2').html(s[1].address);\r\n"
                            "  $('#temp2').html(s[1].temp);\r\n"
                            "  $('#hum2').html(s[1].hum);\r\n"
                            "}\r\n"
                            "}).done(function() {\r\n"
                            "  console.log('ok');\r\n"
                            "})\r\n"
                            "}\r\n"
                            "});"
                            "</script>\r\n"
                            "</body>";

WebServer server(80);

const char* ssid = "SSID";
const char* password = "PASS";

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

void handleRoot() {
  String s = page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
      if (advertisedDevice->haveManufacturerData() == true)
      {
        std::string strManufacturerData = advertisedDevice->getManufacturerData();

        char cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

        if (strManufacturerData.length() == 20 && cManufacturerData[0] == 0x10 && cManufacturerData[1] == 0x00)
        {
          char* manufacturerdata = BLEUtils::buildHexData(NULL, (uint8_t*)advertisedDevice->getManufacturerData().data(), advertisedDevice->getManufacturerData().length());
          String address(advertisedDevice->getAddress().toString().c_str());
          Serial.println();
          Serial.print("Found a Thermobeacon: ");
          Serial.println(address);
          process_ws02(manufacturerdata, address);
          free(manufacturerdata);
        }

      }
      return;
    }

    double value_from_hex_data(const char* service_data, int offset, int data_length, bool reverse, bool canBeNegative = true) {
      char data[data_length + 1];
      memcpy(data, &service_data[offset], data_length);
      data[data_length] = '\0';
      long value;
      if (reverse) {
        // reverse data order
        char rev_data[data_length + 1];
        revert_hex_data(data, rev_data, data_length + 1);
        value = strtol(rev_data, NULL, 16);
      } else {
        value = strtol(data, NULL, 16);
      }
      if (value > 65000 && data_length <= 4 && canBeNegative)
        value = value - 65535;
      //Log.trace(F("value %D" CR), value);
      return value;
    }

    void revert_hex_data(const char* in, char* out, int l) {
      //reverting array 2 by 2 to get the data in good order
      int i = l - 2, j = 0;
      while (i != -2) {
        if (i % 2 == 0)
          out[j] = in[i + 1];
        else
          out[j] = in[i - 1];
        j++;
        i--;
      }
      out[l - 1] = '\0';
    }

    void process_ws02( char* manufacturerdata, String address) {
      temp = value_from_hex_data(manufacturerdata , 24, 4, true) / 16;
      hum = value_from_hex_data(manufacturerdata , 28, 4, true) / 16;

      JsonObject obj = doc.createNestedObject();
      
      obj["address"] = address;
      obj["temp"] = temp;
      obj["hum"] = hum;

      //Limit responses to realistic data. (Thermobeacons have max operating temp. of 65 Degrees C)
      Serial.print("Temperature:");
      Serial.println(temp);
      Serial.print("Humidity:");
      Serial.println(hum);
      Serial.print("Voltage:");
      Serial.println(value_from_hex_data(manufacturerdata , 20, 4, true) / 1000);
    }
};

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Connect to your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  //----------------------------------------------------------------

  server.on("/", handleRoot);      //This is display page
  server.on("/data", []() {
    Serial.println(text);
    server.send(200, "text/plain", text);
    text = "";
  });

  server.begin();                  //Start server
  Serial.println("HTTP server started");


  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.println("Scan done!");
  serializeJson(doc, Serial);
  serializeJson(doc, text);
  doc = doc.to<JsonObject>();
  server.handleClient();
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
