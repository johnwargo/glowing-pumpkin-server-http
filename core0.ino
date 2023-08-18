/*********************************************************
* Core 0
* 
* This is the code that executes on the first core in the 
* ESP32. It's the web server code that responds to remote 
* application requests.
**********************************************************/

// Commands:
// color:# -> disable random colors and set the LED matrix to the selected color
// flash:#x:#y -> disable random colors, flash the lights using the selected color (#x) #y times, enable random colors
// lightning -> disable random colors, flash the lights white (like lightning), enable random colors
// off -> turn off all LEDs
// random -> Enable random colors

void Task0code(void* pvParameters) {
  // Only executes when registering the task on the core
  Serial.print("Web Server running on core ");
  Serial.println(xPortGetCoreID());

  String request, searchStr;
  int color, colorPos, count;

  // This is the worker code for the core, runs infinitely
  // listening for requests from the remote client
  for (;;) {
    request = "";                            // empty this to eliminate previous responses
    WiFiClient client = server.available();  // Listen for incoming clients
    if (client) {
      Serial.println("Client connection");
      while (client.connected()) {  // loop while the client's connected
        if (client.available()) {   // if there's bytes to read from the client,
          char c = client.read();   // read a byte, then
          request += c;             // append it to the response variable

          if (c == '\n') {
            Serial.println(request);

            // Color command
            searchStr = "GET /color:";
            colorPos = searchStr.length();
            if (request.indexOf(searchStr) >= 0) {
              // subtracts '0' from it to get the integer representation of the number
              color = request.charAt(colorPos) - '0';
              Serial.print("Set Color #");
              Serial.println(color);

              if (color > numColors - 1) {  // invalid color idx
                allOff();
                error(client);
                break;
              }

              disableRandom();
              fadeColor(colors[color]);
              success(client);
              break;
            }

            // FLash command
            searchStr = "GET /flash:";
            colorPos = searchStr.length();
            if (request.indexOf(searchStr) >= 0) {
              // subtracts '0' from it to get the integer representation of the number
              color = request.charAt(colorPos) - '0';
              // get the number of flashes
              count = request.charAt(colorPos + 2) - '0';
              Serial.print("Flash color #");
              Serial.print(color);
              Serial.print(", ");
              Serial.print(count);
              Serial.println(" times");

              if (color > numColors - 1) {  // invalid color idx
                allOff();
                error(client);
                break;
              }

              disableRandom();
              flashLEDs(colors[color], count);
              enableRandom();
              success(client);
              break;
            }

            // Lightning command
            if (request.indexOf("GET /lightning") >= 0) {
              Serial.println("Lightning");
              disableRandom();
              flicker();
              enableRandom();
              success(client);
              break;
            }

            // Off command
            if (request.indexOf("GET /off") >= 0) {
              Serial.println("Off");
              allOff();
              success(client);
              break;
            }

            // Random command
            if (request.indexOf("GET /random") >= 0) {
              Serial.println("Random");
              enableRandom();
              success(client);
              break;
            }
          }
        }
        delay(10);
      }
      client.stop();
      Serial.println("client disconnected");
    }
    // Add a small delay to let the watchdog process
    //https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    delay(25);
  }
}

void success(WiFiClient client) {
  Serial.println("Sending Success response");
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.print("{ \"status\": \"success\"}");
  client.println();
}

void error(WiFiClient client) {
  Serial.println("Sending Error response");
  client.println("HTTP/1.1 400 Bad Request");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.print("{ \"status\": \"failure\"}");
  client.println();
}
