/*********************************************************
* Core 0
* 
* This is the code that executes on the first core in the 
* ESP32. It's the web server code that responds to remote 
* application requests.
**********************************************************/

// ESP32 Web Server CORS
// https://stackoverflow.com/questions/65749873/how-to-add-cors-header-to-my-esp32-webserverp32-webserver
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/AdvancedWebServer/AdvancedWebServer.ino
// https://github.com/espressif/arduino-esp32/blob/master/libraries/ESPmDNS/src/ESPmDNS.h

// path args
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/PathArgServer/PathArgServer.ino

// Commands:
// color:# -> disable random colors and set the LED matrix to the selected color
// flash:#x:#y -> disable random colors, flash the lights using the selected color (#x) #y times, enable random colors
// lightning -> disable random colors, flash the lights white (like lightning), enable random colors
// off -> turn off all LEDs
// random -> Enable random colors

#include <ESPmDNS.h>
#include <WebServer.h>

#include <uri/UriBraces.h>

WebServer server(80);

void Task0code(void* pvParameters) {

  Serial.print("Web Server running on core ");
  Serial.println(xPortGetCoreID());

  if (MDNS.begin(HOSTNAME)) {
    displayMessage("MDNS responder started");
    MDNS.addService("http", "tcp", 80);
  } else {
    displayMessage("Error setting up MDNS responder!");
    fadeColor(CRGB::Red);
    while (1) {
      delay(1000);
    }
  }

  server.enableCORS();
  server.on("/", handleRoot);
  server.on(UriBraces("/color:{}"), handleColor);
  server.on(UriBraces("/flash:{}"), handleFlash);
  server.on("/lightning", handleFlicker);
  server.on("/off", handleOff);
  server.on("/random", handleRandom);
  server.onNotFound(handleNotFound);
  server.begin();
  displayMessage("HTTP server started\n");

  for (;;) {
    server.handleClient();
    // Add a small delay to let the watchdog process
    //https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    delay(25);
  }
}

void handleColor() {
  int color;
  String colorStr = server.pathArg(0);

  // Only accept GET requests, this addresses a problem caused by
  // CORS preflight check requests that the web server library
  // doesn't deal with correctly (OK, not at all)
  if (server.method() != HTTP_GET) return;
  displayMessage("color: " + colorStr);

  color = colorStr.toInt();
  if (color > numColors - 1) {  // invalid color idx
    allOff();
    sendError();
    return;
  }
  sendSuccess();
  disableRandom();
  fadeColor(colors[color]);
}


void handleFlash() {
  int color, count;
  String uriParms = server.pathArg(0);

  // Only accept GET requests, this addresses a problem caused by
  // CORS preflight check requests that the web server library
  // doesn't deal with correctly (OK, not at all)
  if (server.method() != HTTP_GET) return;
  displayMessage("flash: " + uriParms);

  color = uriParms.charAt(0) - '0';
  count = uriParms.charAt(2) - '0';
  // invalid color idx or count
  if (color > numColors - 1 || count > 5) {
    sendError();
    allOff();
    return;
  }
  sendSuccess();
  flashLEDs(colors[color], count);
}

void handleFlicker() {
  // Only accept GET requests, this addresses a problem caused by
  // CORS preflight check requests that the web server library
  // doesn't deal with correctly (OK, not at all)
  if (server.method() != HTTP_GET) return;
  displayMessage("Flicker");  // lightning
  sendSuccess();
  flicker();
}

void handleOff() {
  // Only accept GET requests, this addresses a problem caused by
  // CORS preflight check requests that the web server library
  // doesn't deal with correctly (OK, not at all)
  if (server.method() != HTTP_GET) return;
  displayMessage("Off");
  sendSuccess();
  allOff();
}

void handleRandom() {
  // Only accept GET requests, this addresses a problem caused by
  // CORS preflight check requests that the web server library
  // doesn't deal with correctly (OK, not at all)
  if (server.method() != HTTP_GET) return;
  displayMessage("Random");
  sendSuccess();
  enableRandom();
}

void handleRoot() {
  // Send the a web page with a redirect to the hosted pumpkin controller.
  // https://pumpkin-controller.netlify.app/
  // Append the IP address of the server so the app can configure itself
  // and save the IP address for future access.
  displayMessage("Root (/)\n");
  String redirectHTML = "<html>";
  redirectHTML += "<head><title>Redirecting</title>";
  redirectHTML += "<meta http-equiv='Refresh' content=\"3; url='https://pumpkin-controller.netlify.app?" + WiFi.localIP().toString() + "'\" />";
  redirectHTML += "<link rel='stylesheet' href='https://unpkg.com/mvp.css'>";
  redirectHTML += "</head><body><main>";
  redirectHTML += "<h1>Redirecting</h1><p>Redirecting to Pumpkin Controller<p>";
  redirectHTML += "</main></body></html>";
  server.send(200, "text/html", redirectHTML);
}

void handleNotFound() {
  displayMessage("Not Found\n");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void sendSuccess() {
  server.send(200, "application/json", "{ \"status\": \"success\"}");
  // displayMessage("Sent Success response");
}

void sendError() {
  server.send(400, "application/json", "{ \"status\": \"failure\"}");
  displayMessage("Sent Error response\n");
}

void displayMessage(String msg) {
#ifdef DEBUG
  Serial.print("Web server: ");
  Serial.println(msg);
#endif
}
