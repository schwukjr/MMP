#include "House.cpp"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char page[] PROGMEM = R"(
<head>
 <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>
</head>
<body>
 <h1>HomeMonitor: </h1>
 <h2 id=\"text\">""</h2>
 <script>
   $(document).ready(function(){
     setInterval(getData,1000);
     function getData(){
       $.ajax({
         type:\"GET\",
         url:\"data\",
         success: function(data){
           let s = JSON.parse(data);
           let parsedData = "";
           $('#text').html(data);
         }
       }).done(function() {
         console.log('ok');
       })
     }
   });
 </script>
</body>
)";

WebServer server(80);

const char* ssid = "trishypoo";
const char* password = "gingerhair";
String text = "Collecting Data."; //Data to send to web page.

House* house1 = new House("House 1");

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  initialiseWebServer();

  house1->addRoom(new Room("Bedroom"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor1", "b5:70:00:00:06:c4"));
  house1->getRoom("Bedroom")->addSensor(new Thermobeacon("Sensor2", "b5:70:00:00:07:db"));
  house1->addRoom(new Room("Bathroom"));
  house1->getRoom("Bathroom")->addSensor(new OneWireTempSensor("Sensor3", 23));

  text = house1->toJSON();
  Serial.println(text);

}

void loop()
{
  // put your main code here, to run repeatedly:

  server.handleClient();

  //  text = house1->getRoom("Bedroom")->getData();
  //  Serial.println(text);
  //delay(2000);

}

void initialiseWebServer() {
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

  server.on("/", []() {
    server.send(200, "text/html", page);
  });
  server.on("/data", []() {
    //Serial.println(text);
    server.send(200, "text/plain", text);
    //text = "";
  });

  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
