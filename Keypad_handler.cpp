#include <Keypad.h>
#include "Keypad_handler.h"


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

//int Token_Number=0;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}             };

byte rowPins[ROWS] = {26, 27, 12, 13}; // Row pins for the keypad
byte colPins[COLS] = {19, 18, 4, 25};  // Column pins for the keypad
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


char read_one_key_no_interrupt()
{
  while (true) {
    char key = keypad.getKey();
    if(key)
    {
        return key ;
    }
  }
}
  