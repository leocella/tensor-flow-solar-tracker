#include <Arduino.h>

// Defina o pino do LED da sua placa.
// Na maioria das ESP32-S3 DevKitC é o GPIO 2 (azul) ou GPIO 48 (RGB)
#define LED_PINO 2 

void setup() {
    // 1. Pisca o LED imediatamente para provar que o código está rodando
    pinMode(LED_PINO, OUTPUT);
    digitalWrite(LED_PINO, HIGH); // LED acende
    delay(500);
    digitalWrite(LED_PINO, LOW);  // LED apaga
    delay(500);
    digitalWrite(LED_PINO, HIGH); // LED acende (e fica aceso)

    // 2. Inicializa o Serial
    // CRÍTICO: Vamos começar com 9600 para BATER com o driver do Windows
    Serial.begin(115200);
    
    
    
    Serial.println("\n\n=============================");
    Serial.println("CONECTADO! O MONITOR FUNCIONA.");
    Serial.println("=============================");
}

void loop() {
    Serial.print("Estou vivo! Millis: ");
    Serial.println(millis());
    delay(2000); // Envia a cada 2 segundos
}