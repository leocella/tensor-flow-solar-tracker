#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h" // Para IO_TASK_PERIOD_MS e pinos LDR
#include "tasks.h"  // Para a struct SensorData

// Declaração da fila criada em main.cpp
extern QueueHandle_t q_sensor_data;

/**
 * @brief Tarefa de IO (Input/Output)
 * * Esta tarefa é responsável por ler todos os sensores periodicamente
 * e enviar os dados brutos para a fila de controle (q_sensor_data).
 * É uma tarefa periódica que usa vTaskDelayUntil para garantir
 * um tempo de ciclo (jitter) baixo.
 * * Prioridade: 2 (Média)
 * Core: 1
 */
void taskIO(void* arg) {
    // Inicializa a struct de dados
    SensorData data;

    // Inicializa o controle de tempo da tarefa
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    Serial.println("[IO] Tarefa iniciada.");

    for (;;) {
        // ---------- 1. Leitura dos LDRs (0..4095) ----------
        data.ldrTL = analogRead(LDR_TL);
        data.ldrTR = analogRead(LDR_TR);
        data.ldrBL = analogRead(LDR_BL);
        data.ldrBR = analogRead(LDR_BR);
        
        // TODO: Adicionar leitura real da tensão do painel
        data.vPanel = 12.0f; // Valor mockado por enquanto

        // ---------- 2. Envio para Fila de Controle ----------
        // Envia os dados para a fila q_sensor_data
        // O timeout de 10 ticks evita bloquear indefinidamente se a fila estiver cheia
        if (xQueueSend(q_sensor_data, &data, pdMS_TO_TICKS(10)) != pdPASS) {
             Serial.println("[IO] ERRO: Fila de sensor (q_sensor_data) cheia!");
        } else {
             // ---------- 3. Log (Opcional) ----------
             // Serial.println("[IO] Dados dos LDRs enviados para controle.");
        }

        // ---------- 4. Espera Periódica (Não Bloqueante) ----------
        // Garante que esta tarefa rode exatamente a cada IO_TASK_PERIOD_MS
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(IO_TASK_PERIOD_MS));
    }
}