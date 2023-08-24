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
#include <WiFiClient.h>

#include "constants.h"

#define HOSTNAME "pumpkin"
#define DEBUG true
#define NUM_LEDS 25
#define PIN A3

// store the credentials in the project's constants.h file
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

TaskHandle_t Task0;
TaskHandle_t Task1;

// Controls whether random color display is enabled or not.
bool doRandom = false;

// LED Matrix stuff
int numColors = 6;
uint32_t colors[] = { CRGB::Blue, CRGB::Green, CRGB::Orange, CRGB::Purple, CRGB::Red, CRGB::Yellow };
CRGB leds[NUM_LEDS];  // LED Array (internal memory structure from FastLED)

// Set web server port number to 80
// WiFiServer server(80);

void setup() {

  int counter = 0;

  Serial.begin(115200);
  delay(1000);
  Serial.println();
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUM_LEDS);  // Initialize the FastLED library
  flashLEDs(CRGB::Red, 2);                         // Flash the lights RED twice to let everyone know we've initiated
  Serial.print("Connecting to ");
  Serial.println(ssid);
  fadeColor(CRGB::Blue);  // turn all LEDs blue while we connect to the Wi-Fi network

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter += 1;
    if (counter > 25) {
      counter = 0;
      Serial.println();
    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Flash LEDs green to let everyone know we successfully
  // connected to Wi-Fi
  flashLEDs(CRGB::Green, 2);

  disableRandom();

  //create a task that executes the Task0code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(Task0code, "Task0", 10000, NULL, 1, &Task0, 0);
  //create a task that executes the Task0code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 1);
}

void loop() {
  // nothing to do here, everything happens in the Tast1Code and Task2Code functions
}
