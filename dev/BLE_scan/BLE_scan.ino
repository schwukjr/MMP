/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

String text = "";
double temp;
double hum;

const String page PROGMEM = "<head>"
                            " <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
                            " </head>"
                            " <body>"
                            " <h1>Thermobeacon Sensor Data:</h1><h2>Temperature:</h2> <h2 id=\"temp\">""</h2>\r\n"
                            " <h2>Humidity:</h2><h2 id=\"hum\">""</h2>\r\n"
                            " <script>\r\n"
                            " $(document).ready(function(){\r\n"
                            " setInterval(getData,1000);\r\n"
                            " function getData(){\r\n"
                            " $.ajax({\r\n"
                            "  type:\"GET\",\r\n"
                            "  url:\"data\",\r\n"
                            "  success: function(data){\r\n"
                            "  var s = data.split(\'-\');\r\n"
                            "  $('#temp').html(s[0]);\r\n"
                            "  $('#hum').html(s[1]);\r\n"
                            "}\r\n"
                            "}).done(function() {\r\n"
                            "  console.log('ok');\r\n"
                            "})\r\n"
                            "}\r\n"
                            "});"
                            "</script>\r\n"
                            "</body>";

WebServer server(80);

//Enter your SSID and PASSWORD
const char* ssid = "Troy and Abed in the Modem";
const char* password = "m0rn1ngN1ght5";

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

void handleRoot() {
  String s = page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      char* manufacturerdata = BLEUtils::buildHexData(NULL, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
      String sManufacturerData(manufacturerdata);
      if (sManufacturerData.substring(0, 8).equals("10000000")) {
        Serial.println(sManufacturerData);
        process_ws02(manufacturerdata);
      }
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

    void process_ws02( char* manufacturerdata) {
      Serial.print("Temperature:");
      temp = value_from_hex_data(manufacturerdata , 24, 4, true) / 16;
      Serial.println(temp);
      Serial.print("Humidity:");
      hum = value_from_hex_data(manufacturerdata , 28, 4, true) / 16;
      Serial.println(hum);
      Serial.print("Voltage:");
      Serial.println(value_from_hex_data(manufacturerdata , 16, 4, true) / 1000);
      if (temp < 100) {
        server.handleClient();
      }
    }
};

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Connectto your wifi
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
    text = (String)temp;
    text += "-";
    text += (String)hum;
    Serial.println(text);
    server.send(200, "text/plain", text);
  });

  server.begin();                  //Start server
  Serial.println("HTTP server started");


  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}
