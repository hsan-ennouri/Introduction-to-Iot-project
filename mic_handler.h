#ifndef MIC_HANDLER_H
#define MIC_HANDLER_H

#include <Arduino.h> 


void startI2SMicrophone();
void sendAudioData(int16_t *samples, size_t sampleCount);
bool requestTranscription();
bool captureAndSendAudio();
void setup_mic();
bool loop_voice_recognition();


#endif