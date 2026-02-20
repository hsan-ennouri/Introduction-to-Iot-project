#include "function_handler.h"
#include "LCD_handler_to_display.h"
#include "alarm_function.h"
#include "servo_motor_door.h"
#include "Keypad_handler.h"
#include "mqtt_handler.h"
#include <Arduino.h>

int nb_attempts = 1 ;
bool stop = false; 
bool result_password = false ;
String current_code = "";
int counter_pass_char = 0 ;

void init_general_var(){
  nb_attempts = 1 ;
  stop = false; 
  current_code = "";
  result_password = false ;
  counter_pass_char = 0 ;
}


void start_facial_recog(){
  start_facial_recog_LCD() ;
  char result = read_one_key_no_interrupt() ;
}

bool loop_facial_recognition(){
  bool result_facial = false ;
  int counter_facial= 1 ;
  delay(1000);
  while ( (result_facial == false) and (counter_facial <4))  {
    char key = ' ';
    LCD_take_pic_button(counter_facial);
    while (key != 'D'){
      key = read_one_key_no_interrupt();
//      Serial.println("Key pressed is : " + String(key));
    }
    LCD_countdown(" Taking picture");
    start_camera_by_MQTT();
    snapshot_camera();
    capture_complete();
    String message = waitForMQTTMessage();
    if (message == "1") {
      result_facial = true;
//      Serial.println("Success");
    }
    counter_facial+=1;
  }
//  Serial.println(String(result_facial));
  return result_facial;
}

void LCD_start_code(){
  if (stop == false) {
    Set_col_row(get_col(),get_row());
    char key = read_one_key_no_interrupt();
    if (key) {
      if ((key == '0') or (key == '1') or (key == '2') or (key == '3') or (key == '4') or 
          (key == '5') or (key == '6') or (key == '7') or (key == '8') or (key == '9')) {// only numbers
        current_code += key ;
        counter_pass_char += 1 ;
//        Serial.print("Input: ");
//        Serial.println(key);
        display_char(key);
        delay(300);
        increment_col();
      }
      else if ((key == '*') and (counter_pass_char>0)) {
        current_code.remove(current_code.length() - 1);
        Serial.println(current_code);
        counter_pass_char -= 1 ;
        LCD_errase_one();
//        Serial.println("errase");
        delay(300);
      }
      if (counter_pass_char == 4) { // finished entering the actual code
      delay(200);
        if (current_code == PROPOSED_CODE) {
          stop = true ;
          result_password = true ;
        }
        else {
          counter_pass_char = 0 ;
          if (nb_attempts == 3) {
            stop = true ;
            result_password = false ;
          }
          else { 
            current_code = "";
            nb_attempts+=1;
            LCD_display_nb_attempt_code(nb_attempts);
          }
        }
      }
    }
  }
}



void success_to_open(){
  success_to_open_LCD_1();
  delay(100);
  openLock();
  success_to_open_LCD_2();

  char button = ' ' ;
   while (button != 'C') {
    button = read_one_key_no_interrupt();
  }
  closeLock() ;
  init_LCD_var();
}

void failure_to_open(){
  failure_to_open_LCD();
  activate_buzzer();
}

int secret_back_to_sleep(){
//  Serial.print("\n");
//  Serial.print("You're trapped, try to escape now\n");
  while (true) {
//    Serial.print("Let's try to untrapp you \n");
    delay(100);
    int count_keys = 0 ;
    String secret_pass ="";
    char key = ' ';
    while (count_keys<4){
      key = read_one_key_no_interrupt();
      secret_pass+=key;
      delay(100);
//      Serial.print("You pressed : ");
//      Serial.print(key);
//      Serial.print("\n");
      count_keys+=1;
      delay(100);
    }
//    Serial.print("the password you wrote is : " + secret_pass);
//    Serial.print("\n");
    if (secret_pass == TRUE_SECRET_PASSWORD) {
      return 0;
    }
  }
  return 0;
}


bool stop_reading_password(){
  return stop ;
}
bool result_code() {
  return result_password ;
}