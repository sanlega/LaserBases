# 🎯 Arduino Accuracy Game 🕒

Welcome to the Arduino Accuracy Game! This project allows you to create a fun target shooting game using Arduino and ESP32/ESP8266 microcontrollers. The game consists of two main components: Targets and Timer.

## Targets 🎯

The Targets code controls the behavior of each target in the game. Each target is equipped with an IR receiver to detect hits and a NeoPixel LED strip to indicate the hit status. Targets communicate with the Timer using the ESP-NOW protocol.

### Dependencies

- Adafruit_NeoPixel library
- IRremote library
- ESP-NOW library
- WiFi library

### Components Used

- NeoPixel LED strip
- IR receiver module
- ESP32 microcontroller
- ESP8266 microcontroller (optional)

### Configuration 🛠️

- **Target ID:** Change the `TARGET_ID` variable in the code to assign a unique ID to each target.
- **Total Number of Targets:** Update the `totalTargets` variable to match the actual number of targets in your setup.
- **MAC Address:** Manually add the MAC address of the Timer to the `targetMACs` array in the Targets code.

### Usage 🚀

1. Install the required libraries.
2. Upload the code to your ESP32/ESP8266 board.
3. Connect the components according to the pin configuration.
4. Power on the targets and ensure they are within range of the Timer.

## Timer ⏱️

The Timer code serves as the central control unit for the game. It manages the countdown, tracks hits on targets, and determines the end of the game. The Timer communicates with the Targets using the ESP-NOW protocol.

### Dependencies

- ESP-NOW library
- WiFi library

### Components Used

- ESP32 microcontroller

### Configuration 🛠️

- **MAC Address:** Manually add the MAC address of each target to the `targetMACs` array in the Timer code.

### Usage 🚀

1. Upload the code to your ESP32 board.
2. Connect the reset button according to the pin configuration.
3. Power on the Timer and ensure it is connected to the same Wi-Fi network as the Targets.

## Extractor for MAC Address 🖥️

This code snippet can be used to extract the MAC address of an ESP32 or ESP8266 board. It is helpful for obtaining the MAC addresses required for communication between the Timer and Targets.

### Dependencies

- WiFi library

### Usage 🚀

1. Upload the code to your ESP32/ESP8266 board.
2. Open the serial monitor to view the MAC address printed by the board.

