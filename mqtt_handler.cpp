#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "mqtt_handler.h"
#include "LCD_handler_to_display.h"

// Button pin
#define BUTTON_PIN 12 // GPIO pin connected to the button

constexpr char* ssid = "zzz";          // Your WiFi SSID
constexpr char* password = "87654321";   // Your WiFi password 
// HTTP server URL
constexpr char* serverURL = "http://your-server-ip:5000/trigger"; // Replace with your server's address

// MQTT broker details
constexpr char* mqtt_broker = "172.20.10.5";  // Replace with your local broker IP address
const int mqtt_port = 1883;
constexpr char* facial_topic = "facial_recognition/result";
constexpr char* mqttTopic_face = "camera/trigger";  // Topic to trigger the ESP32-CAM
constexpr char* mqttFailedTopic = "camera/failed";

WiFiClient espClient;
PubSubClient client(espClient);

// Flags for keyboard and camera control
bool keyboardEnabled = true;
bool cameraTriggered = false;

bool go_to_password = false;
volatile bool messageReceived = false;
String receivedMessage = ""; // Stores the received message
String targetTopic = ""; // Stores the target topic
// Variables to store results
String facial_result = "";



void init_mqtt_var(){
  keyboardEnabled = true;
  go_to_password = false;
  cameraTriggered = false;
  go_to_password = false;
  messageReceived = false;
  facial_result = "";
  receivedMessage = ""; // Stores the received message
  targetTopic = "";
}

void MQTT_setup() {
    WiFi.begin(ssid, password);
    Serial.println("MQTT SETUP");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
        LCD_connecting_wifi(" Connecting WiFi");
    }

    Serial.println("Connected to WiFi");
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);

    // Connect to MQTT broker
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        LCD_connecting_wifi(" Connecting MQTT");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT broker");
            client.subscribe(facial_topic);  // Subscribe to the facial recognition topic
        } else {
            Serial.print("Failed to connect. Error state: ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void start_camera_by_MQTT() {
  connectToMQTT(mqttTopic_face);
  Serial.println("Trigger 'D' sent. Sending MQTT message...");
  client.publish(mqttTopic_face, "D");

  // Debounce delay
  delay(500);  // Adjust if needed
//  }
}

void send_fail_MQTT(){ 
  connectToMQTT(mqttFailedTopic);
  Serial.println("Sending fail telegramm");
  client.publish(mqttFailedTopic,"failed");
  // Debounce delay
  delay(500);  // Adjust if needed
}

/// Wait for mqtt function  

String waitForMQTTMessage() {
  messageReceived = false;  // Reset the message received flag
  receivedMessage = "";     // Clear any previous message content
  int counter_time_loop = 0 ;
  connectToMQTT(facial_topic);
  client.subscribe(facial_topic);
  LCD_processing();
  
  // Loop until messageReceived is true
  while ((!messageReceived) and (counter_time_loop<150)){
//    client.setCallback(mqttCallback);
    client.loop();  // Maintain the MQTT connection
    delay(100);     // Short delay to avoid a busy wait
    counter_time_loop+=1;
  }
  if (counter_time_loop ==150) { 
    Serial.println("Times up man, too late");
    return String(0);
  }
  Serial.println("Desired message received!" + String(receivedMessage));
  return receivedMessage; // Return the stored message
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Check if the message is for the specific topic we're waiting for
  if (String(topic) == targetTopic) {
    messageReceived = true;

    // Store the received message in `receivedMessage`
    receivedMessage = "";
    for (int i = 0; i < length; i++) {
      receivedMessage += (char)payload[i];
    }
  }
}

void connectToMQTT(const char* topic) {
  Serial.print("here");
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    LCD_connecting_wifi("Connecting MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      LCD_connecting_wifi("  Connected");
      client.subscribe(topic); // Subscribe to the specific topic
      targetTopic = topic; // Set the target topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}
void callback(char* topic, byte* message, unsigned int length) {
  if (strcmp(topic, facial_topic) == 0) {
    receivedMessage = "";
    for (int i = 0; i < length; i++) {
      receivedMessage += (char)message[i];
    }
    messageReceived = true;
//    Serial.println("Message received: " + receivedMessage);
  }
}

bool result_go_to_password(){
  return go_to_password;
}

