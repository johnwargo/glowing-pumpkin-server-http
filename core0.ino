/*********************************************************
* Core 0
* 
* This is the code that executes on the first core in the 
* ESP32. It's the web server code that responds to remote 
* application requests
**********************************************************/

void Task0code(void* pvParameters) {
  // Only executes when registering the task on the core
  Serial.print("Task0 running on core ");
  Serial.println(xPortGetCoreID());

  // This is the worker code for the core, runs infinitely 
  // listening for requests from the remote client
  for (;;) {
    WiFiClient client = server.available();  // Listen for incoming clients
    if (client) {                            // If a new client connects,
      Serial.println("Client connection");
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE html><html>");
      client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
      client.println("<body><h1>ESP32 Web Server</h1>");
      client.println("</body></html>");
      client.println();
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println();
    }
    // Add a small delay to let the watchdog process
    //https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
    delay(25);
  }
}
