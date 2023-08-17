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

  String request;
  int color, count;

  // This is the worker code for the core, runs infinitely
  // listening for requests from the remote client
  for (;;) {
    // Listen for incoming clients
    WiFiClient client = server.available();
    if (!client) return;

    Serial.println("Client connection");
    client.stop();
    Serial.println("client disconnected");

    // while (client.connected()) {
    //   if (client.available()) {
    //     char c = client.read();
    //     request += c;

    //     if (c == '\n') {
    //       Serial.println(request);
    //       // close the connection:
    //       client.stop();
    //       Serial.println("client disconnected");
    //     }

    //     delay(25);
    //   }
    // }
    // Add a small delay to let the watchdog process
    //https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    delay(25);
  }
}

void success() {
}

void error() {
}



// if (c == '\n') {
//   if (request.indexOf("color") != -1) {
//     Serial.println("Color request");
//     color = 2;  // get the color
//     disableRandom();
//     fadeColor(colors[color]);
//   }

//   if (request.indexOf("flash") != -1) {
//     // get the color
//     color = 2;
//     // get the number of flashes
//     count = 3;
//     Serial.print("Flash color #");
//     Serial.print(color);
//     Serial.print(", ");
//     Serial.print(count);
//     Serial.println(" times");
//     disableRandom();
//     flashLEDs(colors[color], count);
//     enableRandom();
//   }

//   if (request.indexOf("lightning") != -1) {
//     Serial.println("Lightning");
//     disableRandom();
//     flicker();
//     enableRandom();
//   }

//   if (request.indexOf("off") != -1) {
//     Serial.println("Off");
//     disableRandom();
//     fadeColor(CRGB::Black);
//   }

//   if (request.indexOf("random") != -1) {
//     Serial.println("Random");
//     enableRandom();
//   }
//   break;
// }



// // If a new client connects,
// Serial.println("Client connection");
// client.println("HTTP/1.1 200 OK");
// client.println("Content-type:text/html");
// client.println("Connection: close");
// client.println();
// client.println("<!DOCTYPE html><html>");
// client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
// client.println("<body><h1>ESP32 Web Server</h1>");
// client.println("</body></html>");
// client.println();
// client.stop();
// Serial.println("Client disconnected.");
// Serial.println();
