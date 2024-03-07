#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_system.h> // Needed for ESP.restart()

#define IR_PIN 25            // IR receiver pin
#define LED_PIN 32           // NeoPixel LED strip pin
#define NUM_PIXELS 16        // Number of pixels in the strip
#define TARGET_ID 1          // Unique ID of the Target
#define DELAY 100            // Assign only to the odd ID's

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
IRrecv irrecv(IR_PIN);
decode_results results;

typedef struct struct_message {
    int id;     // Target ID
    bool hit;   // Hit status
} struct_message;

struct_message myData;

// Timer's ESP32 MAC address
uint8_t timerMAC[] = {0xB0, 0xA7, 0x32, 0x22, 0x6A, 0x30}; // Replace with your Timer's MAC Address

bool targetHit = false; // Flag to track if the target has been hit

void setup() {
  Serial.begin(9600);
  delay(1);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  irrecv.enableIRIn(); // Start the IR receiver

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, timerMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Register callback for receiving ESP-NOW messages
  esp_now_register_recv_cb(OnDataRecv);

  // Initialize all pixels to red (not hit)
  setStripColor(strip.Color(255, 0, 0)); // Red
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFFFFFFF && !targetHit) { // Specific IR code
      setStripColor(strip.Color(0, 255, 0)); // Green
      targetHit = true;

      myData.id = TARGET_ID; // Target ID
      myData.hit = true;
      if (ID % 2 == 0)
        delay(DELAY);
      esp_now_send(timerMAC, (uint8_t *) &myData, sizeof(myData));
    }
    irrecv.resume();
  }
}

void setStripColor(uint32_t color) {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

// Callback function that is called whenever an ESP-NOW message is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  struct_message receivedMsg;
  memcpy(&receivedMsg, incomingData, sizeof(receivedMsg)); // Copy the received data into our struct

  // Check if the received message is a reset command
  if (receivedMsg.id == -1) { // Assuming -1 indicates a reset
    Serial.println("Reset command received. Restarting...");
    delay(100); // Short delay to allow the serial message to be sent out before reset
    ESP.restart(); // Perform a hard reset
  }
}
