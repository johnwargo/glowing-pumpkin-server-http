/*********************************************************
* Core 1
* 
* This is the code that interacts with the LED array, 
* looping infinitely doing the random color thing 
* if enabled.
**********************************************************/

void Task1code(void* pvParameters) {
  Serial.print("Task0 running on core ");
  Serial.println(xPortGetCoreID());

  // Repeat the following infinitely
  for (;;) {
    if doRandom {
      fadeColor(colors[(int)random(1, numColors + 1)]);
    }
  }
}