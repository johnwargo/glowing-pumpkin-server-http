/*********************************************************
* Glowing Pumpkin 
* 
* Xaio ESP32 S3 version using the Adafruit 5x5 BFF LED
* Matrix.  This edition adds a web server and web app
* to the project so you can control it from a smartphone
* or other external device.
*
* By John M. Wargo
* https://johnwargo.com
**********************************************************/

// https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/

#include <FastLED.h>
#include <WiFi.h>

// Local constants
#include "constants.h"

#define DEBUG true
#define NUM_LEDS 25
#define PIN A3

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

TaskHandle_t Task1;
TaskHandle_t Task2;

// don't think I need this
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

int numColors = 6;
uint32_t colors[] = { CRGB::Blue, CRGB::Green, CRGB::Orange, CRGB::Purple, CRGB::Red, CRGB::Yellow };
// LED Array (internal memory structure from FastLED)
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(1000);
  // Initialize the FastLED library
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUM_LEDS);
  // Flash the lights RED twice
  flashLEDs(CRGB::Red, 2, 250);
  delay(1000);
  fadeColor(CRGB::Blue);
  // Connect to Wi-Fi network with SSID and password
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  flashLEDs(CRGB::Blue, 3, 250);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task2",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task2,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  delay(500);
}

void loop() {}

void Task1code(void* pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  // Repeat the following infinitely
  // because that's how this threading thing works.
  for (;;) {
    //generate a random integer between 1 and 10
    if ((int)random(11) > 8) {
      // if it's a 9 or a 10, do that flicker thing
      flicker();
    } else {
      // Otherwise switch to the new color
      fadeColor(colors[(int)random(1, numColors + 1)]);
    }
  }
}

void Task2code(void* pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    WiFiClient client = server.available();  // Listen for incoming clients

    if (client) {  // If a new client connects,
      Serial.println("Client connection");
      String currentLine = "";                                                   // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
        currentTime = millis();
        if (client.available()) {  // if there's bytes to read from the client,
          char c = client.read();  // read a byte, then
          Serial.write(c);         // print it out the serial monitor
          header += c;
          if (c == '\n') {  // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
              client.println("<body><h1>ESP32 Web Server</h1>");
              client.println("</body></html>");
              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else {  // if you got a newline, then clear currentLine
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
        }
      }
      // Clear the header variable
      header = "";
      // Close the connection
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
}

void flashLEDs(CRGB color, int count, int duration) {
  // Calculate this only once, no reason to calculate this in the loop
  int offDuration = duration / 2;

  for (int i = 0; i < count; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(duration);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(offDuration);
  }
}

void flicker() {
  // how many times are we going to flash?
  int flashCount = (int)random(1, 6);
  //flash the lights in white flashCount times
  //with a random duration and random delay between each flash
  for (int i = 0; i < flashCount; i++) {
    // Set all pixels to white and turn them on
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    // Delay for a random period of time (in milliseconds)
    delay((int)random(50, 150));
    //clear the lights (set the color to none)
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    // Delay for a random period of time (in milliseconds)
    delay((int)random(100, 500));
  }
}

// Fill the NeoPixel array with a specific color
void fadeColor(CRGB c) {
  for (int i = 0; i < 25; i++) {
    leds[i] = c;
    FastLED.show();
    delay(10);
  }
  delay((int)random(250, 2000));
}
