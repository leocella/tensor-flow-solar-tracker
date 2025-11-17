#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h" // Para ACTUATOR_TASK_PERIOD_MS
#include "tasks.h"
#include <ESP32Servo.h>

// Declaração dos objetos Servo criados em main.cpp
extern Servo servo1;
extern Servo servo2;

// Declaração da fila criada em main.cpp
extern QueueHandle_t q_servo_cmd;

/**
 * @brief Tarefa de Atuação (Servos)
 * * Move os servos de forma não-bloqueante (rampa).
 * * Espera por comandos na fila q_servo_cmd.
 * * Esta é a única tarefa que pode ter vTaskDelay (curto).
 * * Prioridade: 2 (Média)
 * Core: 0
 */
void taskActuator(void* arg) {
    ControlOutput cmd; // Armazena o comando alvo
    
    // Posições físicas atuais (onde o servo realmente está)
    int currentPos1 = 90;
    int currentPos2 = 90;
    
    // Sincroniza a posição inicial (lendo o primeiro comando)
    if (xQueueReceive(q_servo_cmd, &cmd, portMAX_DELAY) == pdPASS) {
        currentPos1 = cmd.targetPos1;
        currentPos2 = cmd.targetPos2;
        servo1.write(currentPos1);
        servo2.write(currentPos2);
    }
    
    Serial.println("[ACT] Tarefa iniciada e sincronizada.");

    for (;;) {
        // 1. Verifica se há um *novo* comando (não bloqueia)
        // Se houver, atualiza o alvo. Se não, continua indo para o alvo anterior.
        if (xQueueReceive(q_servo_cmd, &cmd, 0) == pdPASS) {
            // Novo alvo recebido
            // Serial.printf("[ACT] Novo alvo S1:%d S2:%d\n", cmd.targetPos1, cmd.targetPos2);
        }

        // 2. Lógica de Rampa (movimento suave)
        // Move 1 passo (VELOCIDADE_SERVO) em direção ao alvo
        if (currentPos1 < cmd.targetPos1) currentPos1 += VELOCIDADE_SERVO;
        else if (currentPos1 > cmd.targetPos1) currentPos1 -= VELOCIDADE_SERVO;

        if (currentPos2 < cmd.targetPos2) currentPos2 += VELOCIDADE_SERVO;
        else if (currentPos2 > cmd.targetPos2) currentPos2 -= VELOCIDADE_SERVO;

        // Garante que a posição atual não ultrapasse os limites físicos
        currentPos1 = constrain(currentPos1, SERVO_1_MIN, SERVO_1_MAX);
        currentPos2 = constrain(currentPos2, SERVO_2_MIN, SERVO_2_MAX);

        // 3. Escreve nos servos
        servo1.write(currentPos1);
        servo2.write(currentPos2);

        // 4. Log (opcional, mas útil para debug)
        // Serial.printf("[ACT] Alvo S1:%d C:%d | S2:%d C:%d | ML:%d\n",
        //     cmd.targetPos1, currentPos1,
        //     cmd.targetPos2, currentPos2,
        //     cmd.useML);

        // 5. Esta é a "velocidade" do servo
        // Equivalente ao seu antigo delay(delayServo)
        vTaskDelay(pdMS_TO_TICKS(ACTUATOR_TASK_PERIOD_MS));
    }
}