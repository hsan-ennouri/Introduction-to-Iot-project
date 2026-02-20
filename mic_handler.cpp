#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>
#include <Arduino.h>
#include "mic_handler.h"
#include "mqtt_handler.h"
#include "LCD_handler_to_display.h"
#include "Keypad_handler.h"
#include "alarm_function.h" 


// INMP441 Microphone Pin Configuration
#define I2S_WS 15   // Word Select (L/R clock)
#define I2S_SD 32   // Serial Data (SD)
#define I2S_SCK 14  // Serial Clock (BCLK)

// Wi-Fi credentials
const char* ssid = "zzz";
const char* password = "87654321";

// Server URLs
const char* transcriptionURL = "http://172.20.10.5:8080/get_transcription";
const char* serverURL_audio = "http://172.20.10.5:8080/upload_audio";


const int sampleRate = 16000;         // Sample rate of 16kHz
const int bufferSize = 1024;          // Buffer size for audio samples
const int captureDuration = 2;        // Capture duration in seconds

//bool isAudioPasswordMatch = false;

void startI2SMicrophone() {
  // I2S configuration
  i2s_config_t i2sConfig = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = bufferSize
  };

  // Set I2S pins for INMP441 based on provided configuration
  i2s_pin_config_t pinConfig = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  // Initialize I2S
  i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pinConfig);
  i2s_set_clk(I2S_NUM_0, sampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void sendAudioData(int16_t *samples, size_t sampleCount) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL_audio);
    http.addHeader("Content-Type", "application/octet-stream");

    // Send audio data as binary
    int httpResponseCode = http.POST((uint8_t*)samples, sampleCount * sizeof(int16_t));

    if (httpResponseCode > 0) {
      Serial.println("Audio chunk sent successfully");
    } else {
      Serial.println("Error sending audio chunk");
      LCD_connecting_wifi("Error audio");
    }

    http.end();
  }
}

bool requestTranscription() {
  bool isAudioPasswordMatch = false;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(transcriptionURL);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Transcription received: " + response);

      // Check if transcription matches the password
      if (response == "hello") {
        isAudioPasswordMatch = true;
        Serial.println("Password match confirmed!");
      } else {
        isAudioPasswordMatch = false;
        Serial.println("Password does not match.");
      }
    } else {
      Serial.println("Error requesting transcription");
    }

    http.end();
    return isAudioPasswordMatch;
  }
}

bool captureAndSendAudio() {
  int16_t samples[bufferSize];
  size_t bytesRead;
  int totalChunks = (sampleRate * captureDuration) / bufferSize;
  Serial.println("Capturing 2 seconds of audio...");
  LCD_voice_2_sec();
  for (int i = 0; i < totalChunks; i++) {
//    Serial.println("Ba3333333");
    i2s_read(I2S_NUM_0, (void*)samples, bufferSize * sizeof(int16_t), &bytesRead, portMAX_DELAY);
//    Serial.println("Gr4guzbcued");

    if (bytesRead > 0) {
      sendAudioData(samples, bytesRead / sizeof(int16_t));
//      Serial.printf("Chunk %d sent.\n", i + 1);
    }
  }
  Serial.println("Audio capture complete. Requesting transcription...");
  capture_complete();
  return requestTranscription(); 
}

void setup_mic() {
  startI2SMicrophone();
  //Serial.println("Press 'r' in the Serial Monitor to start recording 5 seconds of audio.");
}
bool loop_voice_recognition(){
  bool result_voice = false ;
  int counter_voice= 1 ;
  while ( (result_voice == false) and (counter_voice <4))  {
    char key = ' ';
    display_voice(counter_voice );
    while (key != 'D'){
      key = read_one_key_no_interrupt();
      Serial.println("Key pressed is : " + String(key));
    }
    counter_voice+=1;
    LCD_countdown("Recording voice,");
    snapshot_camera();
    result_voice = captureAndSendAudio();
    if (result_voice == true) {
      LCD_voice_success_display();
    }
    else {
      LCD_voice_failure_display();
    }
  }
  return result_voice;
}
