#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <esp_now.h>
#include <WiFi.h>

#define PIN 17 // The data pin for the LED matrix
#define WIDTH 48 // Width of the matrix
#define HEIGHT 16 // Height of the matrix

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

#define BUTTON_PIN 16
#define COUNTDOWN_DURATION 3000

typedef struct struct_message {
  int id;                               // Target ID
  bool hit;                             // Hit status
} struct_message;

unsigned long startTime = 0;
unsigned long countdownStartTime;
bool timerRunning = false;
const int totalTargets = 11;            // Adjust based on your total number of targets
int targetsHit = 0;
bool targets[totalTargets] = { 0 };     // Track which targets have been hit
bool gameOver = false;                  // Flag to track if the game is over
volatile bool resetRequested = false;   // Correctly declare as a global variable


// Array to store the MAC addresses of the target ESP32s
uint8_t targetMACs[totalTargets][6] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

// PROGRAM:
void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(50);
  matrix.setTextColor(matrix.Color(255, 0, 0)); // Red color

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  for (int i = 0; i < totalTargets; i++) {
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, targetMACs[i], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.print("Failed to add peer for target ");
      Serial.println(i);
    }
  }
    
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), requestReset, FALLING);
  sendResetSignalToTargets();
  countdownStartTime = millis();
}

void loop() {
  if (resetRequested) {
    ESP.restart();  // Handle reset outside of ISR
  }

  // During countdown, wait for it to complete before starting the timer
  if (!timerRunning && millis() - countdownStartTime < COUNTDOWN_DURATION) {
    displayCountdown(); // Display the 3-second countdown
    if (millis() - countdownStartTime == COUNTDOWN_DURATION - 1000) {
      Serial.println("Get ready...");
    }
  } else if (!timerRunning && !gameOver) {
    // Countdown finished, start the timer
    timerRunning = true;
    startTime = millis();
    Serial.println("Go!");
  }
    if (timerRunning && !gameOver) {
    displayTimer(millis() - startTime); // Update timer display
    }

  // Check if all targets have been hit and the game is not already over
  if (timerRunning && targetsHit == totalTargets && !gameOver) {
    unsigned long endTime = millis();
    Serial.print("Game Over! Total Time: ");
    Serial.print((endTime - startTime) / 1000.0);
    Serial.println(" seconds.");
    gameOver = true;              // END FLAG
  }
}

void sendResetSignalToTargets() {
  struct_message resetMsg = { -1, false };  // Reset message
  for (int i = 0; i < totalTargets; i++) {
    esp_now_send(targetMACs[i], (uint8_t *)&resetMsg, sizeof(resetMsg));
    delay(200);
  }
  Serial.println("Reset signal sent to all targets.");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  struct_message incomingMessage;
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  if (incomingMessage.hit && !targets[incomingMessage.id]) {
    Serial.print("Target hit: ");
    Serial.println(incomingMessage.id);
    targets[incomingMessage.id] = true;  // Mark target as hit
    targetsHit++;
  }
}

void requestReset() {
  resetRequested = true;  // Set flag in ISR
}

void resetESP() {
  ESP.restart();  // Resets the ESP
}

void displayCountdown() {
  int textSize = 2; 
  matrix.setTextSize(textSize);
  matrix.setTextColor(matrix.Color(255, 255, 255));

  for (int i = 3; i > 0; i--) {
    matrix.fillScreen(0);
    String numStr = String(i);
    int numWidth = 6 * textSize * numStr.length(); 
    int x = (WIDTH - numWidth) / 2 +1;
    int y = (HEIGHT - 8 * textSize) / 2 +1;

    matrix.setCursor(x, y);
    matrix.print(i);
    matrix.show();
    delay(1000);
  }
}

void displayTimer(unsigned long elapsedTime) {
  matrix.setTextSize(1);
  unsigned long seconds = (elapsedTime / 1000) % 60;
  unsigned long minutes = (elapsedTime / (1000 * 60)) % 60;
  unsigned long milliseconds = (elapsedTime % 1000) / 10;

  char buf[20];
  sprintf(buf, "%02lu:%02lu.%02lu", minutes, seconds, milliseconds);
  int textWidth = 6 * strlen(buf);
  matrix.fillScreen(0);
  int x = (WIDTH - textWidth) / 2;
  int y = (HEIGHT - 8) / 2; 
  matrix.setCursor(x, y);
  matrix.print(buf);
  matrix.show();
}
