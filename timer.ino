#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>


// Define the pin for the NeoPixel matrix and the button
#define MATRIX_PIN 6
#define BUTTON_PIN 2

// Define the dimensions of the matrix
#define MATRIX_WIDTH 48
#define MATRIX_HEIGHT 16

// RF24 setup
RF24 radio(7, 8); // CE, CSN pins
RF24Network network(radio);
const uint16_t timer_node = 00;
const uint16_t base_nodes[] = {01, 02};
// const uint16_t base_nodes[] = {01, 02, 03, 04, 05, 011, 012, 013, 014, 015};
const int numBases = 2;
bool baseHit[numBases];

// Initialize NeoPixel matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)};

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SPI.begin();
  radio.begin();
  network.begin(90, timer_node); // channel, node address
  for(int i = 0; i < numBases; i++) {
    baseHit[i] = false;
  }
}

void loop() {
  // Listen for button press to start the game
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(10); // Debounce delay
    if (digitalRead(BUTTON_PIN) == LOW) {
      startGame();
    }
  }

  // Other loop code for RF24, etc.
}

void startGame() {
  // Send reset signal to bases
  resetBases();

  // Start countdown
  for (int i = 3; i > 0; i--) {
    displayNumber(i);
    delay(1000);
  }

  // Start timer
  unsigned long startTime = millis();
  unsigned long currentTime;
  bool allBasesHit = false;

  while (!allBasesHit) {
    currentTime = millis() - startTime;
    displayTime(currentTime);

    // Check for signal from bases
    allBasesHit = checkBasesHit();
  }

  // Stop timer
  displayTime(currentTime);
}

void resetBases() {
  // Send reset signal to all bases
  for(int i = 0; i < numBases; i++) {
    RF24NetworkHeader header(base_nodes[i]);
    bool resetSignal = false;
    network.write(header, &resetSignal, sizeof(resetSignal));
    baseHit[i] = false; // Reset hit status for each base
  }
}

bool checkBasesHit() {
  RF24NetworkHeader header;
  bool hit;
  while (network.available()) {
    network.read(header, &hit, sizeof(hit));
    if (hit) {
      // Mark the base as hit
      int baseIndex = header.from - 1; // Assuming base node IDs start at 1
      if (baseIndex >= 0 && baseIndex < numBases) {
        baseHit[baseIndex] = true;
      }
    }
  }

  // Check if all bases have been hit
  for(int i = 0; i < numBases; i++) {
    if (!baseHit[i]) {
      return false; // If any base is not hit, return false
    }
  }
  return true; // All bases hit
}

void displayNumber(int num) {
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.print(num);
  matrix.show();
}

void displayTime(unsigned long ms) {
  int seconds = (ms / 1000) % 60;
  int minutes = (ms / (1000 * 60)) % 60;
  int hours = (ms / (1000 * 60 * 60)) % 24;

  char buffer[10];
  sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);

  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.print(buffer);
  matrix.show();
}
