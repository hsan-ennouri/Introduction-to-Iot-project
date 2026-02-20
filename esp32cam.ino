#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

// Wi-Fi and MQTT credentials
const char* ssid = "zzz";
const char* password = "87654321";
const char* mqtt_server = "172.20.10.5";
const int mqtt_port = 1883;
const char* mqttTriggerTopic = "camera/trigger";
const char* mqttFailedTopic = "camera/failed";
const char* mqttResultTopic = "facial_recognition/result";
String serverURL = "http://172.20.10.5:8081/upload";

// Telegram BOT settings
String BOTtoken = "7537789668:AAHHIyHNcJOpW3fKlJdCXJ8eFXaJGaRn5rQ";
String CHAT_ID = "1973800422";

bool sendPhoto = false;

WiFiClientSecure clientTCP;
WiFiClient espClient;
UniversalTelegramBot bot(BOTtoken, clientTCP);
PubSubClient mqttClient(espClient);

#define FLASH_LED_PIN 4
bool flashState = LOW;
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Camera Pins (MODEL AI-THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void configInitCamera() {
  Serial.println("Configuring camera...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;
    Serial.println("Camera initialized with high specs.");
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.println("Camera initialized with standard specs.");
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Camera initialized successfully.");

    // Set camera flip vertically
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);  // Flip the camera view vertically
  }
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handling new messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      Serial.println("Unauthorized user attempted to access.");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.print("Received command: ");
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the ESP32-CAM \n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggles flash LED \n";
      bot.sendMessage(CHAT_ID, welcome, "");
      Serial.println("Sent welcome message.");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Flash LED state toggled.");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("Photo request received.");
    }
  }
}

String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";
  int maxRetries = 10;  // Maximum number of retries
  int attempt = 0;

  Serial.println("Capturing photo...");
  camera_fb_t * fb = esp_camera_fb_get();
  esp_camera_fb_return(fb); // dispose initial image
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  

  while (attempt < maxRetries) {
    Serial.printf("Attempt %d: Connecting to Telegram server...\n", attempt + 1);
    if (clientTCP.connect(myDomain, 443)) {
      Serial.println("Connection successful, sending photo...");
      String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
      String tail = "\r\n--RandomNerdTutorials--\r\n";

      size_t imageLen = fb->len;
      size_t extraLen = head.length() + tail.length();
      size_t totalLen = imageLen + extraLen;

      clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
      clientTCP.println("Host: " + String(myDomain));
      clientTCP.println("Content-Length: " + String(totalLen));
      clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
      clientTCP.println();
      clientTCP.print(head);

      uint8_t *fbBuf = fb->buf;
      size_t fbLen = fb->len;
      for (size_t n = 0; n < fbLen; n = n + 1024) {
        if (n + 1024 < fbLen) {
          clientTCP.write(fbBuf, 1024);
          fbBuf += 1024;
        } else if (fbLen % 1024 > 0) {
          size_t remainder = fbLen % 1024;
          clientTCP.write(fbBuf, remainder);
        }
      }

      clientTCP.print(tail);
      esp_camera_fb_return(fb);

      int waitTime = 10000;
      long startTimer = millis();
      while ((startTimer + waitTime) > millis()) {
        delay(100);
        while (clientTCP.available()) {
          char c = clientTCP.read();
          getBody += String(c);
          startTimer = millis();
        }
        if (getBody.length() > 0) break;
      }
      clientTCP.stop();

      if (getBody.length() > 0) {
        Serial.println("Photo sent to Telegram successfully.");
        return getBody;
      } else {
        Serial.println("Failed to receive response from Telegram. Retrying...");
      }
    } else {
      Serial.println("Connection to Telegram server failed. Retrying...");
    }

    attempt++;
    delay(2000);  // Delay before retrying
  }

  Serial.println("Failed to connect to Telegram server after maximum retries.");
  return "Connection to Telegram server failed after max retries.";
}

void sendPhotoToServer() {
  Serial.println("Capturing photo...");
  camera_fb_t * fb = esp_camera_fb_get();
  esp_camera_fb_return(fb); // dispose initial image
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }  

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi is connected. Preparing to send image to server...");
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "image/jpeg");

    int httpResponseCode = http.POST(fb->buf, fb->len);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Image sent to server. HTTP response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Server response: " + response);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);
      if (!error && doc.containsKey("facial_result")) {
        const char* facial_result = doc["facial_result"];
        Serial.print("Facial result received: ");
        Serial.println(facial_result);

        // Attempt to connect to MQTT for up to 10 seconds
        unsigned long startAttemptTime = millis();
        while (!mqttClient.connected() && millis() - startAttemptTime < 10000) {
          reconnectMQTT();
        }

        if (mqttClient.connected()) {
          mqttClient.publish(mqttResultTopic, facial_result);
          Serial.println("Facial result published to MQTT.");
        } else {
          Serial.println("MQTT not connected. Failed to publish facial result after 10 seconds.");
        }
      } else {
        Serial.println("JSON parsing failed or 'facial_result' not found in server response.");

        // Attempt to connect to MQTT for up to 10 seconds
        unsigned long startAttemptTime = millis();
        while (!mqttClient.connected() && millis() - startAttemptTime < 10000) {
          reconnectMQTT();
        }

        if (mqttClient.connected()) {
          mqttClient.publish(mqttResultTopic, "0");
          Serial.println("No face published to MQTT.");
        } else {
          Serial.println("MQTT not connected. Failed to publish facial result after 10 seconds.");
        }
      }
    } else {
      Serial.print("Error sending image to server. HTTP response code: ");
      Serial.println(httpResponseCode);

      // Publish "0" to MQTT if there is an HTTP error
      unsigned long startAttemptTime = millis();
      while (!mqttClient.connected() && millis() - startAttemptTime < 10000) {
        reconnectMQTT();
      }

      if (mqttClient.connected()) {
        mqttClient.publish(mqttResultTopic, "0");
        Serial.println("Error message '0' published to MQTT.");
      } else {
        Serial.println("MQTT not connected. Failed to publish error message after 10 seconds.");
      }
    }
    http.end();
  } else {
    Serial.println("Wi-Fi is not connected. Cannot send image to server.");
  }

  esp_camera_fb_return(fb);
  Serial.println("Photo capture and server upload process completed.");
}



void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received message on topic '");
  Serial.print(topic);
  Serial.print("': ");
  Serial.println(message);

  if (String(topic) == mqttTriggerTopic && message == "D") {
    Serial.println("Trigger message received. Initiating photo capture and server upload...");
    sendPhotoToServer();
    bot.sendMessage(CHAT_ID, "An attempt was made", "");
    sendPhotoTelegram();
  }

  if (String(topic) == mqttFailedTopic && message == "failed") {
    Serial.println("Failed authentication detected. Initiating photo capture and Telegram upload...");
    bot.sendMessage(CHAT_ID, "Intruder Alert!", "");
    sendPhotoTelegram();
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32_CAM_Client")) {
      Serial.println("Connected to MQTT broker.");
      mqttClient.subscribe(mqttTriggerTopic);
      mqttClient.subscribe(mqttFailedTopic);
      Serial.print("Subscribed to topics: ");
      Serial.print(mqttTriggerTopic);
      Serial.print(", ");
      Serial.println(mqttFailedTopic);
    } else {
      Serial.print("MQTT connection failed. Error code: ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);


  Serial.println("Initializing...");
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  configInitCamera();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi.");

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  if (sendPhoto) {
    Serial.println("Triggering photo send...");
    sendPhotoTelegram();
    sendPhoto = false;
  }

  // Additional Serial Monitor input handling for "D" and "F"
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'D') {
      Serial.println("Simulating MQTT 'D' trigger - starting photo capture and server upload.");
      sendPhotoToServer();
      bot.sendMessage(CHAT_ID, "An attempt was made", "");
      sendPhotoTelegram();
    } else if (input == 'F') {
      Serial.println("Simulating MQTT 'failed' trigger - starting photo capture and Telegram upload.");
      bot.sendMessage(CHAT_ID, "Intruder Alert!", "");
      sendPhotoTelegram();
    }
  }

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("Received Telegram message.");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
