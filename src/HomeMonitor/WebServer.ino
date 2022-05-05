const char page[] PROGMEM = R"=="==(
<!DOCTYPE HTML><html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  
  <title>HomeMonitor</title>
  
  <script src="https://code.jquery.com/jquery-1.7.1.min.js"></script>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
</head>
<body>
  <div class="container">
    <div class="row align-items-start">
      <div class="col">
        <h1>HomeMonitor:</h1>
      </div>
    </div>
    <div class="row align-items-start">
      <div class="col">
        <form action="/get">
          <div class="row">
            <div class="col">
              <p>Source JSON:</p>
            </div>
          </div>
          <div class="row">
            <div class="col">
              <textarea type="text" id="sourcejson" name="sourcejson" rows="24" cols="60"></textarea>
            </div>
          </div>
          <div class="row">
            <div class="col">
              <div class="btn-group-vertical" role="group" aria-label="Basic example">
//                <input type="button" id="btnprettify" value="Format" class="btn btn-secondary">
                <input type="submit" value="Submit" class="btn btn-primary">
              </div>
            </div>
          </div>
        </form>
      </div>
      <hr>
      <div class="col">
        <h3>System Messages:</h3>
        <textarea type="text" id="msgs" name="msgs" rows="24" cols="75" disabled readonly>
          No System Messages.
        </textarea>  
      </div>
      <hr>
    </div>
    <div class="row align-items-start">
      <div class="col">
        <h2>Current Values:</h2><div id="text"></div>
      </div>
      <div class="col">
        <p><a href="/toggle"><button class="btn btn-success">Enable/Disable System</button></a></p>
        <p><a href="/clear_rooms"><button class="btn btn-primary">Clear Rooms</button></a></p>
      </div>
    </div>
  </div>
  
  <script>
    $(document).ready(function(){
      setInterval(getData,3000);
      setInterval(getSystemMessages,3000);
//      document.getElementById ("btnprettify").addEventListener ("click", prettifyJson, false);
      load();
      function load(){
        $.ajax({
          type:"GET",
          url:"house",
          success: function(data){        
            $('#sourcejson').html(data);
          }  
        }) 
      }
      
//      function prettifyJson(){ // Derived from https://stackoverflow.com/a/26324037
//        var uglyJson = document.getElementById("sourcejson").value;
//        var jsonObject = JSON.parse(uglyJson);
//        var prettyJson = JSON.stringify(jsonObject, undefined, 4);
//        document.getElementById('sourcejson').value = prettyJson;
//      }
      
      function getData(){
        $.ajax({
          type:"GET",
          url:"data",
          success: function(data){  
            let s = JSON.parse(data);
            let parsedData = "";
            parsedData += "<p>House Name: " + s.name + "</p>";
            parsedData += "<p>Number of Rooms: " + s.numberofrooms + "</p>";
            parsedData += "<p>Rooms:</p>";
            
            for (let i = 0; i < s.numberofrooms; i++) {
              parsedData += "<textarea type=\"text\" id=\"sourcejson\" name=\"sourcejson\" rows=\"18\" cols=\"60\">"
              parsedData += "--- Room Name: " + s.rooms[i].name + "\n";
              parsedData += "--- Average Temperature: " + s.rooms[i].averagetemperature + "\n";
              parsedData += "--- Average Humidity: " + s.rooms[i].averagehumidity + "\n\n";
              parsedData += "--- Number of Sensors: " + s.rooms[i].numberofsensors + "\n";
              for (let j = 0; j < s.rooms[i].numberofsensors; j++){
                parsedData += "------ Sensor Name: " + s.rooms[i].sensors[j].name + "\n";
                parsedData += "------ Type: " + s.rooms[i].sensors[j].type + "\n";
                parsedData += "------ Measured Temperature: " + s.rooms[i].sensors[j].temperature + "\n";
                parsedData += "------ Measured Humidity: " + s.rooms[i].sensors[j].humidity + "\n\n";
              }
              
              parsedData += "--- Number of Cycles: " + s.rooms[i].numberofcycles + "\n";
              for (let j = 0; j < s.rooms[i].numberofcycles; j++){
                parsedData += "------ Type: " + s.rooms[i].cycles[j].type + "\n";
                parsedData += "------ Goal: " + s.rooms[i].cycles[j].goal + "\n";
                parsedData += "------ Start Time: " + s.rooms[i].cycles[j].starttime + "\n";
                parsedData += "------ End Time: " + s.rooms[i].cycles[j].endtime + "\n";
                parsedData += "------ Active Days: " + s.rooms[i].cycles[j].activedays + "\n";
                parsedData += "------ Active: " + s.rooms[i].cycles[j].active + "\n\n";
              }
              parsedData += "--- Number of Controls: " + s.rooms[i].numberofcontrols + "\n";
              for (let j = 0; j < s.rooms[i].numberofcontrols; j++){
                parsedData += "------ Name: " + s.rooms[i].controls[j].name + "\n";
                parsedData += "------ Type: " + s.rooms[i].controls[j].type + "\n";
                parsedData += "------ Additive System: " + s.rooms[i].controls[j].additivesystem + "\n";
                parsedData += "------ Enabled: " + s.rooms[i].controls[j].enabled + "\n\n";
              }
              parsedData += "</textarea>";
              parsedData += "<br>";
            }
            $('#text').html(parsedData);
          }
        }).done(function() {
          console.log('ok');
        })
      }
     
      function getSystemMessages(){
        $.ajax({
          type:"GET",
          url:"messages",
          success: function(data){  
      
            $('#msgs').html(data);
          }  
        }) 
      }
    });
  </script>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p" crossorigin="anonymous"></script>
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
  });

//  server.on("/house", HTTP_GET, [](AsyncWebServerRequest * request) {
//    xSemaphoreTake(mutex, portMAX_DELAY);
//    String sHouseJson = preferences.getString("houseJson", "");
//    xSemaphoreGive(mutex);
//    char houseJson[sHouseJson.length() + 1] = "";
//    sHouseJson.toCharArray(houseJson, sHouseJson.length() + 1);
//    request->send_P(200, "text/plain", houseJson);
//  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("sourcejson")) {
      inputMessage = request->getParam("sourcejson")->value();
      inputParam = "sourcejson";
      xSemaphoreTake(mutex, portMAX_DELAY);
      house1->constructFromJson(inputMessage);
      //preferences.putString("houseJson", inputMessage);
      xSemaphoreGive(mutex);
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println("Constructed house using JSON");
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
