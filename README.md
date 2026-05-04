# Wi-Spy32

<p align="center">
  <img width="450" height="450" alt="LOGO" src="https://github.com/user-attachments/assets/b4c15cea-d225-411b-a1a5-b45b6485bab3"/>
  <br>
  <b>ESP32-based WiFi real-time RSSI visualizer</b>
  <br><br>

  <img src="https://img.shields.io/badge/ESP32-microcontroller-red?logo=espressif"/>
  <img src="https://img.shields.io/badge/status-completed-brightgreen?logo=checkmarx"/>
  
</p>

## Description
<b>RSSI</b> (Received Signal Strength Indicator) is a measurement of the power level of a received radio signal. In wireless systems like WiFi, it is typically expressed in dBm (decibels referenced to 1 milliwatt). Because it represents received power, values are usually negative: for example, around −30 dBm indicates a very strong signal, while values below −80 dBm correspond to weak or unreliable reception. <br><br>
The goal of this tool is, once connected, to monitor the received signal strength using an OLED display.
<br>

## Component & Schematic
The component needed for this project are:
* ESP-WROOM-32 Devkit V1
* Button
* Led
* SSD1306 128x32 I2C OLED display

<img width="836" height="672" alt="image" src="https://github.com/user-attachments/assets/99735d80-05ee-4982-bb2f-8af24ab2a608" />

<br>

## Code & Settings
The libraries needed are:
* Adafruit SSSD1306 and Adafruit GFX Library
* ESP32 WiFi libraries (usually already installed in the IDE)

Before uploading the code to the ESP32, some settings and preferences can be adjusted in the `config.h` file.
