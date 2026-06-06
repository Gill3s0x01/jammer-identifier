#include <WiFi.h>
#include "esp_wifi.h"
#include <LedControl.h>

#define DIN_PIN 23
#define CLK_PIN 18
#define CS_PIN 5

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 2);

volatile uint32_t packetCount = 0;
int current_channel = 1;

void IRAM_ATTR sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  packetCount++;
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 2; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 4);
    lc.clearDisplay(i);
  }
  WiFi.mode(WIFI_PROMISC);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
}

void loop() {
  esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
  packetCount = 0;
  delay(40); 
  
  uint32_t count = packetCount; 
  int height = map(count, 0, 80, 0, 8); 
  height = constrain(height, 0, 8); 

  drawBar(current_channel - 1, height);

  current_channel++;
  if (current_channel > 14) current_channel = 1;
}

void drawBar(int col, int height) {
  int device = (col < 8) ? 0 : 1;
  int localCol = col % 8;
  for (int row = 0; row < 8; row++) {
    bool state = (row < height);
    lc.setLed(device, 7 - row, localCol, state);
  }
}