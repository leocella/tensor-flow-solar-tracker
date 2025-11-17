#pragma once

// ========== MODO DE CONTROLE ==========
// 1 = usa TinyML; 0 = usa regra original (LDRs)
#define USE_ML 1

// ========== DEFINIÇÕES DE PINOS ==========
#define LDR_TL 4
#define LDR_TR 5
#define LDR_BL 7
#define LDR_BR 6
#define SERVO_1_PIN 13 // Vertical (Inclinação)
#define SERVO_2_PIN 14 // Horizontal (Azimute)
#define ENCODER_PIN_A 17 // Usar ESP32Encoder (pino A)
#define ENCODER_PIN_B 18 // Usar ESP32Encoder (pino B) - RECOMENDADO

// ========== LIMITES MECÂNICOS ==========
#define SERVO_1_MIN 50  // (Inclinação)
#define SERVO_1_MAX 100 // (Inclinação)
#define SERVO_2_MIN 0   // (Azimute)
#define SERVO_2_MAX 180 // (Azimute)

// ========== PARÂMETROS DE CONTROLE ==========
#define IO_TASK_PERIOD_MS 100
#define ACTUATOR_TASK_PERIOD_MS 15 // Equivalente ao seu delayServo
#define LIMIAR_MOVIMENTO 50
#define VELOCIDADE_SERVO 1 // Passo de 1 grau na rampa
#define LUZ_MINIMA_STOP 0.02f
#define ALPHA_FILTRO 0.30f