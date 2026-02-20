#include "sleep_handler.h"
#include "LCD_handler_to_display.h"
#include "esp_sleep.h"
#include <Arduino.h>


#define WAKEUP_PIN GPIO_NUM_35
void enter_sleep_mode() {

  LCD_init_sleep();

  Serial.println("Woke up from deep sleep!");

  // Configure GPIO 35 to wake up when it goes HIGH
  esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); // 1 means wake up on HIGH signal
  
  // Print message and enter deep sleep
  Serial.println("Going to deep sleep now. Press the button to wake up.");
  delay(500); // Allow time for the message to be sent

  // Enter deep sleep
  esp_deep_sleep_start();

}