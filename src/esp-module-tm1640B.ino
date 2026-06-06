#include <WiFi.h>
#include "esp_wifi.h"
#include <TM1640.h>
#include <TM16xxMatrix.h>

// Definição dos pinos conectados ao ESP32
#define DIN_PIN 23
#define CLK_PIN 18

// Inicializa o módulo TM1640 (Não precisa de pino CS)
TM1640 module(DIN_PIN, CLK_PIN);

// Configura a Matriz de LED (Módulo, Colunas, Linhas)
TM16xxMatrix matrix(&module, 16, 8);

// Variável para contagem de pacotes
volatile uint32_t packetCount = 0;
int current_channel = 1;

// Callback: Toda vez que um pacote WiFi cruza o ar, isso é executado
void IRAM_ATTR sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  packetCount++;
}

void setup() {
  Serial.begin(115200);

  // Inicializa o Display (true = ligado, 2 = nível de brilho de 0 a 7)
  module.setupDisplay(true, 2); 
  matrix.clear(); // Limpa a tela

  // Ativa o rádio no modo de escuta
  WiFi.mode(WIFI_PROMISC);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
  
  Serial.println("[INFO] Detector de Jammer TM1640 Iniciado!");
}

void loop() {
  // Pula para o canal atual
  esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);

  packetCount = 0;
  delay(40); // Tempo escutando o canal
  
  uint32_t count = packetCount; 

  // Mapeamento para os 8 LEDs de altura. (Ajuste o 80 para calibrar a sensibilidade)
  int height = map(count, 0, 80, 0, 8); 
  height = constrain(height, 0, 8); 

  drawBar(current_channel - 1, height);

  current_channel++;
  if (current_channel > 14) {
    current_channel = 1;
  }
}

// Função para desenhar a barra no TM1640
void drawBar(int col, int height) {
  // Primeiro, apaga todos os LEDs daquela coluna específica
  for(int row = 0; row < 8; row++) {
     matrix.setPixel(col, row, false); 
  }
  
  // Depois, acende os LEDs correspondentes à intensidade do tráfego
  for (int row = 0; row < height; row++) {
     // ATENÇÃO: Dependendo de qual lado é o "topo" da sua matriz física, 
     // pode ser necessário mudar "7 - row" para apenas "row".
     matrix.setPixel(col, 7 - row, true);
  }
}