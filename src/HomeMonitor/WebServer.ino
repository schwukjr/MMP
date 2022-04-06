const String page PROGMEM = "<head>"
                            " <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
                            " </head>"
                            " <body>"
                            " <h1>HomeMonitor: </h1><div id=\"text\">"
                            " </div>\r\n"
                            " <script>\r\n"
                            " $(document).ready(function(){\r\n"
                            " setInterval(getData,1000);\r\n"
                            " function getData(){\r\n"
                            " $.ajax({\r\n"
                            "  type:\"GET\",\r\n"
                            "  url:\"data\",\r\n"
                            "  success: function(data){\r\n" //  "  \r\n"

                            "  let s = JSON.parse(data);\r\n"
                            "  let parsedData = \"\";\r\n"

                            "  parsedData += \"<p>House Name: \" + s.housename + \"</p>\";\r\n"
                            "  parsedData += \"<p>Number of Rooms: \" + s.numberofrooms + \"</p>\";\r\n"
                            "  parsedData += \"<p>Rooms:</p>\";\r\n"

                            "  for (let i = 0; i < s.numberofrooms; i++) {\r\n"
                            "   parsedData += \"<p> --- Room Name: \" + s.rooms[i].roomname + \"</p>\";\r\n"
                            "   parsedData += \"<p> --- Average Temperature: \" + s.rooms[i].averagetemperature + \"</p>\";\r\n"
                            "   parsedData += \"<p> --- Average Humidity: \" + s.rooms[i].averagehumidity + \"</p>\";\r\n"
                            "   parsedData += \"<p> --- Number of Sensors: \" + s.rooms[i].numberofsensors + \"</p>\";\r\n"
                            "   parsedData += \"<p> --- Sensors:</p>\";\r\n"

                            "   for (let j = 0; j < s.rooms[i].numberofsensors; j++){\r\n"
                            "     parsedData += \"<p> ------ Sensor Name: \" + s.rooms[i].sensors[j].sensorname + \"</p>\";\r\n"
                            "     parsedData += \"<p> ------ Measured Temperature: \" + s.rooms[i].sensors[j].temperature + \"</p>\";\r\n"
                            "     parsedData += \"<p> ------ Measured Humidity: \" + s.rooms[i].sensors[j].humidity + \"</p>\";\r\n"
                            "     parsedData += \"<br>\";\r\n"
                            "   }\r\n"
                            "   parsedData += \"<br>\";\r\n"
                            "  }\r\n"

                            "  $('#text').html(parsedData);\r\n"
                            "}\r\n"
                            "}).done(function() {\r\n"
                            "  console.log('ok');\r\n"
                            "})\r\n"
                            "}\r\n"
                            "});"
                            "</script>\r\n"
                            "</body>";


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

  server.on("/", []() {
    server.send(200, "text/html", page);
  });
  server.on("/data", []() {
    //Serial.println(text);
    server.send(200, "text/plain", text);
    //text = "";
  });

  server.begin();  //Start server
  Serial.println("HTTP server started");
}
