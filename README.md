# 🎮 Arduino-Based Accuracy Test Game

## Overview 🌟
This project presents a physical accuracy test game using Arduino, RF communication, and NeoPixels. It includes ten interactive bases with IR sensors and a central timer unit. The objective is to hit all bases as quickly as possible. When a base is hit, its LED ring lights change from red to green 🚦, and the timer stops once all bases are hit, displaying the player's time ⏱️.

## Hardware Requirements 🛠️
- X Arduino (one for each base)
- RF24 modules for wireless communication 📡
- IR sensors for hit detection 🎯
- NeoPixel 24 LEDs Rings 💡
- 3 NeoPixel 16x16 LED matrix (16x48 pixel's display)

- 3D printed cases (The timer case is divided in 2 parts that have to be glued in. If using an smaller matrix, there are many cases online)

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
3. Upload `timer.ino` to the timer unit Arduino.
4. Power up the system and test.

## Configuring Base Addresses 🌐

### Unique Node Addressing
Each base in the game needs a unique node address to communicate correctly with the central timer. This is set by the `const uint16_t this_node` line in the `base.ino` code.

### How to Set Unique Addresses
- In the `base.ino` file, locate the line `const uint16_t this_node = 01;`.
- The `01` in this line is the unique address for the first base. 
- For each additional base, you will need to assign a different address.
- Follow a sequential pattern for ease of organization. For example:
  - Base 1: `const uint16_t this_node = 01;`
  - Base 2: `const uint16_t this_node = 11;`
  - Base 3: `const uint16_t this_node = 21;`
  - And so on, up to Base 10: `const uint16_t this_node = 91;`.

### Important Note 📝
Each base can handle up to 5 sub-nodes. Therefore, addresses should be incremented in tens (01, 11, 21, etc.), allowing space for potential sub-nodes under each main base node.
