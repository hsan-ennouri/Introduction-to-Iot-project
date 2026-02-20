#ifndef FUNCTION_HANDLER_H
#define FUNCTION_HANDLER_H

#define TRUE_SECRET_PASSWORD "****" 
#define PROPOSED_CODE "0123" 

// init
void init_general_var();

//Functions
void LCD_start_code();


// Return general value function
void start_facial_recog() ;
bool loop_facial_recognition();
void success_to_open();
void failure_to_open() ;

int secret_back_to_sleep();


// Return General value functions
bool stop_reading_password();
bool result_code() ;
#endif