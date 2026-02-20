#include <ESP32Servo.h>
#include "servo_motor_door.h"
#include <Arduino.h>

constexpr int servoPin = 33; 
Servo lockServo;


void servo_init() {
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	lockServo.setPeriodHertz(50);    // standard 50 hz servo
	lockServo.attach(servoPin, 0, 2500);
  lockServo.write(0); 
}

void openLock() {
  lockServo.write(40);  // Rotate to unlock position
  delay(3000);
  Serial.println("opening");

}

void closeLock() {
  lockServo.write(0);  // Rotate to unlock position
  delay(3000);
  Serial.println("closing");
}

/*void sendAlert() {
  Serial.println("Intrusion detected! Sending alert...");
}*/