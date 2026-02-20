#include "esp_sleep.h"
#include "alarm_function.h"
#include "LCD_handler_to_display.h"
#include "servo_motor_door.h"
#include "mqtt_handler.h"
#include "Keypad_handler.h"
#include "sleep_handler.h"
#include "function_handler.h"
#include "mic_handler.h"

using namespace std;
 
#define WAKEUP_PIN GPIO_NUM_12

void setup() {
  Serial.begin(115200);
  Serial.print("start now");
  buzzer_setup();
  servo_init();
  delay(1000);
  init_general_var();
  Serial.print("\n Let's start this new day\n");
  init_LCD_var() ;
  init_mqtt_var() ;
  MQTT_setup();
  Begin_prog_LCD();
}


void loop() {
  char result = read_one_key_no_interrupt() ;
  while ( (result != 'B') and (result != 'A') ) {
    result = read_one_key_no_interrupt() ;
  }
  if (result == 'B') {
    enter_sleep_mode();
  }
  else if (result == 'A') {
    start_facial_recog() ; 
    bool result_facial = loop_facial_recognition();
    if (result_facial == false) {
      LCD_init_password();
      while (stop_reading_password() == false){
        LCD_start_code();
      }
      bool resultCode = result_code() ;
      if (resultCode == true) {
        success_to_open();
        enter_sleep_mode();
      }
      else {
        send_fail_MQTT();
        failure_to_open();
        int var = secret_back_to_sleep();
        enter_sleep_mode();
      }
    }
    else {
//      Serial.println("Face is ok");
      setup_mic();
      bool result_voice =loop_voice_recognition();
      if (result_voice == false) {
        LCD_init_password();
        while (stop_reading_password() == false){
          LCD_start_code();
        }
        bool resultCode = result_code() ;
        if (resultCode == true) {
          success_to_open();
          enter_sleep_mode();
        }
        else {
          send_fail_MQTT();
          failure_to_open();
          int var = secret_back_to_sleep();
          enter_sleep_mode();
        }
      }
      else {
        success_to_open();
        enter_sleep_mode();
      }
    }
  }
}
