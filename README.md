# IoT Biometric Smart Vault
## Overview
Engineered a secure, internet-connected physical vault as part of the CS3237 Introduction to IoT course at the National University of Singapore (NUS). Designed the complete mechanical enclosure, integrated the hardware components, and developed the local embedded firmware. The system utilizes an ESP32 microcontroller to handle multi-factor authentication, physical actuation, and cloud connectivity.

## Key Features
Multi-Factor Authentication: Integrates a 4x4 matrix keypad, facial recognition and speech to text transcription for secure user verification.

Custom Locking Mechanism: Actuates a custom-designed 3D-printed sliding-bolt lock using a precision servo motor.

Real-Time Feedback: Provides clear user instructions and system status updates via an integrated LCD screen.

Power Management: Implements embedded state machines for system initialization and low-power sleep modes to conserve energy.

Cloud Integration: Communicates with IoT cloud services to log access attempts and monitor vault status remotely.

## Hardware Components
ESP32 Microcontroller

4x4 Matrix Keypad

LCD Display (I2C)

Servo Motor

INMP441 Microphone

Active buzzer

ESP32-CAM

Custom 3D Printed Enclosure and Mechanical Fasteners

## Software Architecture
Developed the core control logic in C++ utilizing the Arduino framework. Architected the firmware around a robust finite state machine (FSM) to handle concurrent hardware tasks without blocking the main execution loop. The FSM reliably manages transitions between IDLE, AUTHENTICATING, UNLOCKED, and SLEEP states based on sensor inputs and network triggers.

## Mechanical Design
Modeled the entire vault enclosure and the internal sliding-bolt locking mechanism using CATIA. Optimized the mechanical components to house the electronics securely while ensuring smooth actuation of the servo motor. Exported the CAD assemblies and manufactured the prototype using 3D printing technologies.
