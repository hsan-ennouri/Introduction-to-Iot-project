#ifndef LCD_HANDLER_TO_DISPLAY.H
#define LCD_HANDLER_TO_DISPLAY.H

#include <Arduino.h>

// var initialisation
void init_LCD_var() ;
// LCD general functions to be used 
void LCD_original_init();
void Set_col_row(int col, int row);
int get_col();
int get_row();
void display_char(char key) ;
void increment_col();

//LCD display functions for specific cases
void Begin_prog_LCD();
void LCD_init_sleep();
void start_facial_recog_LCD() ;
void LCD_take_pic_button(int counter_facial);
void LCD_init_password();
void LCD_display_nb_attempt_code(int nb_attempt);
void success_to_open_LCD_1();
void success_to_open_LCD_2();
void failure_to_open_LCD() ;
void display_voice(int counter_voice );
void LCD_voice_success_display();
void LCD_voice_failure_display();
void LCD_processing();
void LCD_fail();
void LCD_connecting_wifi(String phrase);
void LCD_countdown(String phrase);
void LCD_voice_2_sec();
void capture_complete();

// functionnal functions 
void LCD_errase_one();
void LCD_write_and_second_column(String sentense);
void LCD_print(char current_letter);
#endif