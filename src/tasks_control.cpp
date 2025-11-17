#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h"
#include "tasks.h"
// #include "tf_inference.h" // (Ainda não necessário)
#include <math.h>

// Declaração das filas criadas em main.cpp
extern QueueHandle_t q_sensor_data;
extern QueueHandle_t q_servo_cmd;

// Protótipos locais (para a lógica de Regra que virá depois)
int controlarServo1_regra(int diferenca, int posAtual);
int controlarServo2_regra(int diferenca, int posAtual);

// Estado interno da tarefa (posições-alvo atuais)
static int posServo1_atual = 90;
static int posServo2_atual = 90;

/**
 * @brief Tarefa de Controle
 * * Espera por dados na fila q_sensor_data.
 * * Processa os dados (Regra ou ML).
 * * Envia os ângulos-alvo para a fila q_servo_cmd.
 * * Prioridade: 3 (Alta)
 * Core: 1
 */
void taskControl(void* arg) {
    SensorData data;
    ControlOutput cmd;
    
    Serial.println("[CTRL] Tarefa iniciada. Aguardando dados dos sensores...");

    for (;;) {
        // 1. Espera por novos dados da taskIO (bloqueia até receber)
        if (xQueueReceive(q_sensor_data, &data, portMAX_DELAY) == pdPASS) {

            // Log: Imprime que recebeu os dados do LDR
            // Serial.printf("[CTRL] LDR TL:%d, TR:%d, BL:%d, BR:%d\n", 
            //     data.ldrTL, data.ldrTR, data.ldrBL, data.ldrBR);

            // 2. Processamento (TODO: Implementar lógica de Regra aqui - Dia 2)
            // Por enquanto, apenas mantém a posição em 90 graus
            #if USE_ML
                // (Lógica ML virá aqui - Dia 3)
            #else
                // (Lógica de Regra virá aqui - Dia 2)
                // Vamos apenas definir um alvo fixo por enquanto
                posServo1_atual = 90; 
                posServo2_atual = 90;
            #endif

            
            // 3. Envia comando para a taskActuator
            cmd.targetPos1 = posServo1_atual;
            cmd.targetPos2 = posServo2_atual;
            cmd.useML = USE_ML; // Passa o modo atual para o atuador (para log)

            if (xQueueSend(q_servo_cmd, &cmd, pdMS_TO_TICKS(10)) != pdPASS) {
                 Serial.println("[CTRL] ERRO: Fila de comando (q_servo_cmd) cheia!");
            }
        }
    }
}


/* --- Funções de Lógica (a serem implementadas no Dia 2) --- */

int controlarServo1_regra(int diferenca, int posAtual) {
    // TODO: Implementar lógica de regra (Dia 2)
    // if (abs(diferenca) > LIMIAR_MOVIMENTO) { ... }
    return posAtual; 
}

int controlarServo2_regra(int diferenca, int posAtual) {
    // TODO: Implementar lógica de regra (Dia 2)
    return posAtual; 
}