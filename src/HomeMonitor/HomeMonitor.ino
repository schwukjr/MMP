#include "House.cpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const String page PROGMEM = "<head>"
                            " <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
                            " </head>"
                            " <body>"
                            " <h1>Thermobeacon Sensor Data: </h1><h2>Temperature:</h2> <h2 id=\"temp1\">""</h2>\r\n"
                            " <h2>Humidity:</h2><h2 id=\"hum1\">""</h2>\r\n"
                            " <script>\r\n"
                            " $(document).ready(function(){\r\n"
                            " setInterval(getData,1000);\r\n"
                            " function getData(){\r\n"
                            " $.ajax({\r\n"
                            "  type:\"GET\",\r\n"
                            "  url:\"data\",\r\n"
                            "  success: function(data){\r\n"
                            "  let s = JSON.parse(data);\r\n"
                            "  $('#temp1').html(s.temp);\r\n"
                            "  $('#hum1').html(s.hum);\r\n"
                            "}\r\n"
                            "}).done(function() {\r\n"
                            "  console.log('ok');\r\n"
                            "})\r\n"
                            "}\r\n"
                            "});"
                            "</script>\r\n"
                            "</body>";


WebServer server(80);

const char* ssid = "trishypoo";
const char* password = "gingerhair";
String text = ""; //Data to send to web page.

void handleRoot() {
  String s = page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

House* house1 = new House("House 1");

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Connect to your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
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

  house1->addRoom(new Room("Bedroom"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor1", "b5:70:00:00:06:c4"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor2", "b5:70:00:00:07:db"));
  house1->getRoom("Bedroom")->addSensor(new OneWireTempSensor("Sensor3", 23));


}

void loop()
{
  // put your main code here, to run repeatedly:

  text = house1->getRoom("Bedroom")->getData();
  Serial.println(text);
  server.handleClient();
  delay(2000);

}
