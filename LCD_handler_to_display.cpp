#include "LCD_handler_to_display.h"
#include "Keypad_handler.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
int compt_col = 0 ;
int compt_row = 1 ;

void init_LCD_var(){
  compt_col = 0 ;
  compt_row = 1 ;
}
void LCD_original_init(){
  lcd.init();
  lcd.backlight();
  Set_col_row(0,0);
}
void Set_col_row(int col, int row){
  compt_col = col ;
  compt_row = row ;
  lcd.setCursor(compt_col, compt_row);
}
int get_col(){
  return compt_col;
}
int get_row(){
  return compt_row;
}
void display_char(char key){
  lcd.print(key);
}
void increment_col(){
  compt_col+=1 ;
}


void Begin_prog_LCD(){
  LCD_original_init();
  lcd.print(" Sleep Mode : B ");
  Set_col_row(0,1);
  lcd.print("   Start : A  ");
}
void LCD_init_sleep() {
  LCD_original_init();
  lcd.print("   Sleep Mode   ");
  Set_col_row(0,1);
  lcd.print("Press the button");
  delay(1000);
}
void start_facial_recog_LCD(){
  LCD_original_init();
  lcd.print("Press any button");
  Set_col_row(0,1);
  lcd.print("  to scan face  ");
}
void LCD_take_pic_button(int counter_facial){
//  Serial.println("This is attempt " + String(counter_facial));
  delay(400);
  LCD_original_init();
  lcd.print("Attempt number:"+String(counter_facial));
  Set_col_row(0,1);
  lcd.print("Press D for pic.");
}
void LCD_init_password() {
  LCD_original_init();
  lcd.print("Attempt failed !");
  Set_col_row(0,1);
  lcd.print(" Enter password ");
  delay(3000);
  LCD_original_init();
  lcd.print("Attempt number:1");
  Set_col_row(6,1);
}
void LCD_display_nb_attempt_code(int nb_attempt){
  LCD_original_init();
  lcd.print("Attempt number:"+String(nb_attempt));
  Set_col_row(6,1);
}
void success_to_open_LCD_1(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  congrats !!!");
//  Serial.println("good");
}
void success_to_open_LCD_2(){
  LCD_original_init();
  lcd.print("Press C to close");
  Set_col_row(0,1);
  lcd.print("    the door     ");
}
void failure_to_open_LCD(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    thief !!");
}
void display_voice(int counter_voice ){
//  Serial.println("This is attempt " + String(counter_voice));
  delay(400);
  LCD_original_init();
  lcd.print("Attempt number:"+String(counter_voice));
  Set_col_row(0,1);
  lcd.print("  D to record.  ");
}
void LCD_voice_success_display(){
  LCD_original_init();
  lcd.print("    Password     ");
  Set_col_row(0,1);
  lcd.print("    recognized  ");
}
void LCD_voice_failure_display(){
  LCD_original_init();
  lcd.print("Fail to recognize");
  Set_col_row(0,1);
  lcd.print("  The password  ");
}
void LCD_processing(){
  LCD_original_init();
  lcd.print("  processing");
}
void LCD_fail(){
  LCD_original_init();
//  Serial.println("      fail      ");
  lcd.print("      fail      ");
}
void LCD_connecting_wifi(String phrase){
  LCD_original_init();
  lcd.print(phrase);
}
void LCD_countdown(String phrase){
  for (int i = 3; i>=1 ; --i){
    LCD_original_init();
    lcd.print(phrase);
    Set_col_row(0,1);
    lcd.print("      in "+ String(i));
    delay(250);
  }
  delay(500);
}
void LCD_voice_2_sec(){
  LCD_original_init();
  lcd.print("Recording for");
  Set_col_row(0,1);
  lcd.print(" 2  seconds.");
}
void capture_complete(){
  LCD_original_init();
  lcd.print("Capture complete");
}


// functionnal functions 
void LCD_errase_one(){
    compt_col -= 1 ;
    lcd.setCursor(compt_col, compt_row);
    lcd.print(" ");
}

void LCD_print(char current_letter){
  lcd.print(current_letter); // Display the character on the LCD
  delay(500);
}

void LCD_write_and_second_column(String sentense){
  LCD_original_init();
  lcd.print(sentense);
  Set_col_row(0,1);
}