#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 16               // GPIO pin used for the reset button
#define COUNTDOWN_DURATION 3000     // 3 seconds countdown
#define TARGETS 3                   // Set as many targets as you have

typedef struct struct_message {
    int id;    // Target ID
    bool hit;  // Hit status
} struct_message;

const int totalTargets = TARGETS;

unsigned long startTime = 0;
unsigned long countdownStartTime;
bool timerRunning = false;
int targetsHit = 0;
bool targets[totalTargets] = {0};  // Track which targets have been hit
bool gameOver = false;
volatile bool resetRequested = false;


// Array to store the MAC addresses of the target ESP32s
uint8_t targetMACs[totalTargets][6] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // REPLACE with mac address of each target (here 3 targets example)
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

void setup() {
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
    ESP.restart(); // Handle reset outside of ISR
  }
  // During countdown, wait for it to complete before starting the timer
  if (!timerRunning && millis() - countdownStartTime < COUNTDOWN_DURATION) {
    // Waiting for countdown to finish
    if (millis() - countdownStartTime == COUNTDOWN_DURATION - 1000) { // Approximately 1 second before countdown ends
      Serial.println("Get ready...");
    }
  } else if (!timerRunning && !gameOver) {
    // Countdown has finished, start the timer
    timerRunning = true;
    startTime = millis();
    Serial.println("Go!");
  }

  // Check if all targets have been hit and the game is not already over
  if (timerRunning && targetsHit == totalTargets && !gameOver) {
    unsigned long endTime = millis();
    Serial.print("Game Over! Total Time: ");
    Serial.print((endTime - startTime) / 1000.0);
    Serial.println(" seconds.");
    gameOver = true; // Mark the game as over to prevent re-entering this block
    // Optionally, reset the ESP here to start a new game automatically
    // ESP.restart(); // Uncomment to auto-restart after game ends
  }
}

void sendResetSignalToTargets() {
  struct_message resetMsg = {-1, false}; // Reset message
  for (int i = 0; i < totalTargets; i++) {
    esp_now_send(targetMACs[i], (uint8_t *)&resetMsg, sizeof(resetMsg));
  }
  Serial.println("Reset signal sent to all targets.");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  struct_message incomingMessage;
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  if (incomingMessage.hit && !targets[incomingMessage.id]) {
    Serial.print("Target hit: ");
    Serial.println(incomingMessage.id);
    targets[incomingMessage.id] = true; // Mark target as hit
    targetsHit++;
  }
}

void requestReset() {
  resetRequested = true; // Set flag in ISR
}

void resetESP() {
  ESP.restart(); // Resets the ESP
}
