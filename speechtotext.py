from flask import Flask, request
import wave
import os
import time
import threading
import speech_recognition as sr

app = Flask(__name__)

AUDIO_SAMPLE_RATE = 16000
AUDIO_SAMPLE_WIDTH = 2
AUDIO_CHANNELS = 1
TARGET_DURATION = 2
MARGIN = 2048

audio_data = bytearray()
audio_lock = threading.Lock()
recognizer = sr.Recognizer()

def save_audio_file(filename, data):
    try:
        with wave.open(filename, 'wb') as wf:
            wf.setnchannels(AUDIO_CHANNELS)
            wf.setsampwidth(AUDIO_SAMPLE_WIDTH)
            wf.setframerate(AUDIO_SAMPLE_RATE)
            wf.writeframes(data)
        print(f"Audio saved to {filename}")
    except Exception as e:
        print(f"Error saving audio file: {e}")

def transcribe_audio(filename):
    try:
        with sr.AudioFile(filename) as source:
            audio = recognizer.record(source)
            text = recognizer.recognize_google(audio)
            print(f"Transcription: {text}")
            return text
    except sr.UnknownValueError:
        print("Could not understand the audio")
        return "Could not understand the audio"
    except sr.RequestError as e:
        print(f"Error with recognition service: {e}")
        return "Error with the recognition service"

@app.route('/upload_audio', methods=['POST'])
def upload_audio():
    global audio_data

    with audio_lock:
        audio_data.extend(request.data)
        print(f"Received chunk size: {len(request.data)} bytes. Total buffer size: {len(audio_data)} bytes.")

    return "Chunk received", 200

@app.route('/get_transcription', methods=['GET'])
def get_transcription():
    global audio_data
    required_data_size = TARGET_DURATION * AUDIO_SAMPLE_RATE * AUDIO_SAMPLE_WIDTH

    with audio_lock:
        if len(audio_data) >= required_data_size - MARGIN:
            filename = f"captured_audio_{int(time.time())}.wav"
            save_audio_file(filename, audio_data)
            transcription = transcribe_audio(filename)
            audio_data.clear()
            return transcription, 200
        else:
            return "Insufficient audio data", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
