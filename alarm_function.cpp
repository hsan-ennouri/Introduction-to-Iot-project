#include "alarm_function.h" 
#include <Arduino.h>

constexpr int buzzerPin = 16;

void buzzer_setup(){
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin,HIGH);
}

void activate_buzzer(){
  Serial.println("Let's buzz" );
  for (int i = 0 ; i < 10 ; ++i){
    digitalWrite(buzzerPin, LOW);  // Turn buzzer on
    delay(500);              // Wait for 500ms
    digitalWrite(buzzerPin, HIGH);   // Turn buzzer off
    delay(500);  
  }
  digitalWrite(buzzerPin,HIGH);// Turn buzzer off
}

void snapshot_camera(){
  digitalWrite(buzzerPin, LOW);  // Turn buzzer on
    delay(100);
    digitalWrite(buzzerPin, HIGH);   // Turn buzzer off
    delay(100); 
}