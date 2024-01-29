# 🎮 Arduino-Based Accuracy Test Game

## Overview 🌟
This project presents a physical accuracy test game using Arduino, RF communication, and NeoPixels. It includes ten interactive bases with IR sensors and a central timer unit. The objective is to hit all bases as quickly as possible. When a base is hit, its LED ring lights change from red to green 🚦, and the timer stops once all bases are hit, displaying the player's time ⏱️.

## Hardware Requirements 🛠️
- X Arduino (one for each base)
- RF24 modules for wireless communication 📡
- IR sensors for hit detection 🎯
- NeoPixel 24 LEDs Rings 💡
- 3 NeoPixel 16x16 LED matrix (16x48 pixel's display)

- 3D printed cases.

## Gameplay 🕹️
- **Start:** The game starts with all bases displaying red lights 🔴.
- **Action:** Players hit the bases, which are detected by IR sensors.
- **Scoring:** Upon hitting a base, its lights turn green 🟢.
- **End:** The game ends when all bases are hit, and the timer displays the total time taken 🏁.

## Base Unit Code (`base.ino`) 💻
This code controls the individual bases. Each base has an IR sensor and a Ring of NeoPixels. The base listens for hits via the IR sensor. When hit, it communicates with the central timer via RF24 to log the event.

### Key Features
- IR sensor integration for hit detection 🎯.
- NeoPixel control for visual feedback (red for inactive, green for hit) 🚦.
- RF24 communication to signal the central timer 📡.

## Timer Unit Code (`timer.ino`) ⏲️
The timer unit tracks the game's progress. It uses an Adafruit NeoMatrix to display the time and communicates with each base to determine when they are hit.

### Key Features
- LED Matrix display for the timer ⏱️.
- RF24 network to receive signals from bases 📡.
- Button to start/reset the game 🔄.

## Installation 📋
1. Assemble the hardware according to the schematics.
2. Upload `base.ino` to each base unit Arduino.
    Modify the
    ```const uint16_t this_node = 01; // Unique node address for this base  | IMPORTANT: Each node can hanle 5 sub nodes, After 01 goes 11, 21, 31, 41 and 51.```
3. Upload `timer.ino` to the timer unit Arduino.
4. Power up the system and test.

## Contributing 🤝
Feel free to fork this project and contribute to its development. Your ideas for improvements or new features are always welcome!
