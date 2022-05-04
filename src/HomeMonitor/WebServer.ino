const char page[] PROGMEM = R"=="==(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
  <script src="https://code.jquery.com/jquery-1.7.1.min.js"></script>
</head>

<body>
  <h1>HomeMonitor:</h1>
  <br>
  <form action="/get">
  Source JSON: <input type="text" name="sourcejson">
  <input type="submit" value="Submit">
  </form>
  <br>
  <p><a href="/toggle"><button class="button">Enable/Disable System</button></a></p>
  <br>
  <h2>Current Values:</hr><div id="text"></div>

  <script>
    $(document).ready(function(){
      setInterval(getData,1000);
      function getData(){
        $.ajax({
          type:"GET",
          url:"data",
          success: function(data){  
            if (data != ""){          
              let s = JSON.parse(data);
              let parsedData = "";
  
              parsedData += "<p>House Name: " + s.housename + "</p>";
              parsedData += "<p>Number of Rooms: " + s.numberofrooms + "</p>";
              parsedData += "<p>Rooms:</p>";
              for (let i = 0; i < s.numberofrooms; i++) {
                parsedData += "<p> --- Room Name: " + s.rooms[i].roomname + "</p>";
                parsedData += "<p> --- Average Temperature: " + s.rooms[i].averagetemperature + "</p>";
                parsedData += "<p> --- Average Humidity: " + s.rooms[i].averagehumidity + "</p>";
                parsedData += "<p> --- Number of Sensors: " + s.rooms[i].numberofsensors + "</p>";
                parsedData += "<p> --- Sensors:</p>";
  
                for (let j = 0; j < s.rooms[i].numberofsensors; j++){
                  parsedData += "<p> ------ Sensor Name: " + s.rooms[i].sensors[j].sensorname + "</p>";
                  parsedData += "<p> ------ Measured Temperature: " + s.rooms[i].sensors[j].temperature + "</p>";
                  parsedData += "<p> ------ Measured Humidity: " + s.rooms[i].sensors[j].humidity + "</p>";
                  parsedData += "<br>";
                }
                parsedData += "<br>";
              }
              $('#text').html(parsedData);
            }
          }
        }).done(function() {
          console.log('ok');
        })
      }
    })
  </script>
</body>
</html>)=="==";


void initialiseWebServer() {
  WiFi.mode(WIFI_STA);  //Connect to your wifi
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

  //HTTP Server Responses
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", page);
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", text);
    Serial.println("Sending data to webpage!" + String(text));
  });
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("sourcejson")) {
      inputMessage = request->getParam("sourcejson")->value();
      inputParam = "sourcejson";
      xSemaphoreTake(mutex, portMAX_DELAY);
      //Construct from JSON
      xSemaphoreGive(mutex);
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send_P(200, "text/html", page);
  });
  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (house1->maintainingState) {
      Serial.println("System Disabled.");
      house1->maintainingState = false;
    } else {
      Serial.println("System Enabled.");
      house1->maintainingState = true;
    }
    request->send_P(200, "text/html", page);

  });
  //  server.on("/disable", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    request->send_P(200, "text/html", page);
  //  });

  server.begin();  //Start server
  Serial.println("HTTP server started");
}
