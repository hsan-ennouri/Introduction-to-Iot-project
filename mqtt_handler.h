#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H
#include <Arduino.h>

void init_mqtt_var() ;
void MQTT_setup();
void start_camera_by_MQTT();
void send_fail_MQTT();

//Wait for Mqtt functions
String waitForMQTTMessage(/*const char* topic*/);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void connectToMQTT(const char* topic);

// General use functions
void callback(char* topic, byte* message, unsigned int length);

//Return variables functions
bool result_go_to_password();

#endif