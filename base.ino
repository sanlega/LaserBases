#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include <Adafruit_NeoPixel.h>

#define PIN            6  // Pin for NeoPixel
#define NUMPIXELS      24 // Number of NeoPixels
#define IR_SENSOR_PIN  2  // Pin for IR Sensor

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(7, 8); // CE, CSN pins for RF24
RF24Network network(radio);
const uint16_t this_node = 01; // Unique node address for this base  | IMPORTANT: Each node can hanle 5 sub nodes, After 01 goes 11, 21, 31, 41 and 51.
const uint16_t timer_node = 00; // Timer node address

bool hit = false; // Variable to store hit status

void setup() {
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
  pinMode(IR_SENSOR_PIN, INPUT);

  SPI.begin();
  radio.begin();
  network.begin(90, this_node); // Channel, node address
}

void loop() {
  network.update();
  RF24NetworkHeader header;

  // Check if the IR sensor is hit
  if (digitalRead(IR_SENSOR_PIN) == HIGH) {
    if (!hit) { // Debounce check
      hit = true;
      // Change LED color to red
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red color
        pixels.show();
      }

      // Send a signal to the timer
      header.to = timer_node;
      network.write(header, &hit, sizeof(hit));
    }
  } else {
    hit = false; // Reset hit status when sensor is not activated
  }

  // Listen for a reset signal from the timer
  while (network.available()) {
    network.read(header, &hit, sizeof(hit));
    if (!hit) {
      // Reset the base
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green color
        pixels.show();
      }
    }
  }
}
