// src/tasks_control.cpp
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h"
#include "tasks.h"
#include <math.h>

// Filas criadas em main.cpp
extern QueueHandle_t q_sensor_data;
extern QueueHandle_t q_servo_cmd;

// Estado interno da tarefa (posições-alvo atuais)
static int posServo1_atual = 90;
static int posServo2_atual = 90;

/**
 * @brief Tarefa de Controle (Cérebro)
 */
void taskControl(void* arg) {
    SensorData data;
    ControlOutput cmd;
    
    Serial.println("[CTRL] Tarefa iniciada. Aguardando dados dos sensores...");

    for (;;) {
        // 1. Espera por novos dados da taskIO (bloqueia até receber)
        if (xQueueReceive(q_sensor_data, &data, portMAX_DELAY) == pdPASS) {

            // 2. Loga dados recebidos (Conforme Checklist)
            Serial.printf("[CTRL] Processando... LDRs(TL/TR/BL/BR): %d, %d, %d, %d\n", 
                data.ldrTL, data.ldrTR, data.ldrBL, data.ldrBR);

            // 3. Processamento (STUB - Comando Fixo)
            // (No Dia 2, a lógica de Regra e Coleta de Dados entra aqui)
            
            // Stub: manter em 90 graus
            posServo1_atual = 90; 
            posServo2_atual = 90;

            // 4. Envia comando para a taskActuator
            cmd.targetPos1 = posServo1_atual;
            cmd.targetPos2 = posServo2_atual;
            cmd.useML = USE_ML;

            if (xQueueSend(q_servo_cmd, &cmd, pdMS_TO_TICKS(10)) != pdPASS) {
                 Serial.println("[CTRL] ERRO: Fila de comando (q_servo_cmd) cheia!");
            }
        }
    }
}