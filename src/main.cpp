#include <Arduino.h>
#include <ESP32Servo.h>
#include <math.h>

// ========== MODO DE CONTROLE ==========
// 1 = usa TinyML (ml_controller.h); 0 = usa regra original (LDRs)
#define USE_ML 0

#if USE_ML
  #include "ml_controller.h"   // requer /ml/model.h gerado
#endif

// ========== DEFINIÇÕES DE PINOS ==========
// LDRs em QUADRANTES (conforme seu mapeamento):
// TL = Top-Left (superior esquerdo)   -> GPIO 4
// TR = Top-Right (superior direito)   -> GPIO 5
// BL = Bottom-Left (inferior esquerdo)-> GPIO 7
// BR = Bottom-Right (inferior direito)-> GPIO 6
#define LDR_TL 4
#define LDR_TR 5
#define LDR_BL 7
#define LDR_BR 6

// Servos (PWM)
#define SERVO_1_PIN 13   // Servo Motor 1 (eixo vertical)
#define SERVO_2_PIN 14   // Servo Motor 2 (eixo horizontal)

// Encoder
#define ENCODER_PIN 17   // Canal A (contagem simples)

// ========== OBJETOS ==========
Servo servo1;
Servo servo2;

// ========== VARIÁVEIS DE CONTROLE ==========
int posServo1 = 90;     // posição inicial (0..180)
int posServo2 = 90;

int ldrTL, ldrTR, ldrBL, ldrBR;
int difVertical, difHorizontal;      // diferenças já suavizadas para controle

// suavização (filtro 1ª ordem)
float dv_f = 0.0f, dh_f = 0.0f;
const float alpha = 0.30f;           // 0..1 (maior = responde mais rápido)

unsigned long ultimoTempo = 0;
const unsigned long intervaloLeitura = 100;  // ms entre leituras

// Encoder
volatile int encoderPulsos = 0;

// ========== CONFIGURAÇÕES DE SENSIBILIDADE ==========
const int   limiarMovimento = 50;    // zona morta para decidir movimento
const int   velocidadeServo = 2;     // graus por passo
const int   delayServo      = 15;    // ms entre passos
const float luzMinimaStop   = 0.02f; // congela sob baixa luz (0..1)

// ========== PROTÓTIPOS ==========
void IRAM_ATTR contadorEncoder();
void controlarServo1_regra(int diferenca);
void controlarServo2_regra(int diferenca);
void exibirDados();

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== ESP32-S3 | Solar Tracker LDR (Quadrantes) + Servo + Encoder ===");

  // Servos
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  servo1.write(posServo1);
  servo2.write(posServo2);
  Serial.println("Servos inicializados na posição 90°");

  // Encoder
  pinMode(ENCODER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), contadorEncoder, RISING);

#if USE_ML
  ml_begin();
  Serial.println("TinyML inicializado.");
#else
  Serial.println("Modo: controle por regra (baseline).");
#endif

  Serial.println("Sistema pronto! Lendo LDRs e controlando servos...");
}

// ========== LOOP ==========
void loop() {
  const unsigned long agora = millis();

  if (agora - ultimoTempo >= intervaloLeitura) {
    ultimoTempo = agora;

    // ---------- Leitura dos LDRs (0..4095) ----------
    ldrTL = analogRead(LDR_TL);
    ldrTR = analogRead(LDR_TR);
    ldrBL = analogRead(LDR_BL);
    ldrBR = analogRead(LDR_BR);

    // ---------- Combinações por faixa/lado ----------
    const int avgTop    = (ldrTL + ldrTR) / 2;
    const int avgBottom = (ldrBL + ldrBR) / 2;
    const int avgLeft   = (ldrTL + ldrBL) / 2;
    const int avgRight  = (ldrTR + ldrBR) / 2;

    // ---------- Diferenças "físicas" corretas ----------
    const int difV_raw = avgTop  - avgBottom; // + => mais luz em cima
    const int difH_raw = avgLeft - avgRight;  // + => mais luz à esquerda

    // ---------- Suavização ----------
    dv_f = alpha * (float)difV_raw + (1.0f - alpha) * dv_f;
    dh_f = alpha * (float)difH_raw + (1.0f - alpha) * dh_f;
    difVertical   = (int)lroundf(dv_f);
    difHorizontal = (int)lroundf(dh_f);

    // ---------- Baixa luz: congela ----------
    const float soma_norm = (ldrTL + ldrTR + ldrBL + ldrBR) / (4.0f * 4095.0f);
    if (soma_norm < luzMinimaStop) {
      exibirDados();
      return;
    }

#if USE_ML
    // ---------- Controle por TinyML ----------
    float d1 = 0.0f, d2 = 0.0f;
    ml_infer((float)difVertical, (float)difHorizontal,
             (float)posServo1, (float)posServo2, soma_norm,
             d1, d2);

    // Zona morta e clamp
    const float dead = 0.25f;
    if (fabsf(d1) < dead) d1 = 0.0f;
    if (fabsf(d2) < dead) d2 = 0.0f;

    if (d1 >  3.0f) d1 =  3.0f; if (d1 < -3.0f) d1 = -3.0f;
    if (d2 >  3.0f) d2 =  3.0f; if (d2 < -3.0f) d2 = -3.0f;

    const int novo1 = constrain(posServo1 + (int)lroundf(d1), 50, 100);
    const int novo2 = constrain(posServo2 + (int)lroundf(d2), 0, 180);

    if (novo1 != posServo1) { posServo1 = novo1; servo1.write(posServo1); delay(delayServo); }
    if (novo2 != posServo2) { posServo2 = novo2; servo2.write(posServo2); delay(delayServo); }

    // (opcional) mistura com regra para robustez em cantos extremos:
    // int d1_rule = (abs(difVertical)   > limiarMovimento) ? (difVertical   > 0 ? velocidadeServo : -velocidadeServo) : 0;
    // int d2_rule = (abs(difHorizontal) > limiarMovimento) ? (difHorizontal > 0 ? velocidadeServo : -velocidadeServo) : 0;
    // posServo1 = constrain(posServo1 + (int)lroundf(0.3f * d1_rule), 0, 180);
    // posServo2 = constrain(posServo2 + (int)lroundf(0.3f * d2_rule), 0, 180);

#else
    // ---------- Controle por REGRA (baseline) ----------
    controlarServo1_regra(difVertical);
    controlarServo2_regra(difHorizontal);
#endif

    // ---------- Logs ----------
    exibirDados();
  }
}

// ========== FUNÇÕES DE CONTROLE (REGRA) ==========
void controlarServo1_regra(int diferenca) {
  if (abs(diferenca) > limiarMovimento) {
    // +dif => mais luz em cima => sobe (se inverter na mecânica, troque os sinais)
    posServo1 = constrain(posServo1 + (diferenca > 0 ? velocidadeServo : -velocidadeServo), 50, 100);
    servo1.write(posServo1);
    delay(delayServo);
  }
}

void controlarServo2_regra(int diferenca) {
  if (abs(diferenca) > limiarMovimento) {
    // +dif => mais luz à esquerda => vai para a esquerda
    posServo2 = constrain(posServo2 + (diferenca > 0 ? velocidadeServo : -velocidadeServo), 0, 180);
    servo2.write(posServo2);
    delay(delayServo);
  }
}

// ========== ENCODER ==========
void IRAM_ATTR contadorEncoder() {
  encoderPulsos++;
}

// ========== LOG ==========
void exibirDados() {
  Serial.print("LDR TL: "); Serial.print(ldrTL);
  Serial.print(" | TR: ");  Serial.print(ldrTR);
  Serial.print(" | BL: ");  Serial.print(ldrBL);
  Serial.print(" | BR: ");  Serial.print(ldrBR);

  Serial.print(" | dV: ");  Serial.print(difVertical);
  Serial.print(" dH: ");     Serial.print(difHorizontal);

  Serial.print(" | S1: ");   Serial.print(posServo1); Serial.print("°");
  Serial.print(" S2: ");     Serial.print(posServo2); Serial.print("°");

  Serial.print(" | Enc: ");  Serial.print(encoderPulsos);
  Serial.println();
}
