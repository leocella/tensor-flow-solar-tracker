// src/main.cpp
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "config.h"
#include "tasks.h"
#include <ESP32Servo.h>

// Globais: Servos (usados pela taskActuator)
Servo servo1;
Servo servo2;

// Globais: Filas (o "encanamento" entre as tarefas)
QueueHandle_t q_sensor_data;
QueueHandle_t q_servo_cmd;

void setup() {
    // 1. Inicializar Serial
    Serial.begin(115200);
    
    // REMOVIDO: while (!Serial); 
    // O FreeRTOS agora iniciará imediatamente.
    
    delay(500); // Um pequeno delay para o boot
    Serial.println("\n\n=== ESP32-S3 | Solar Tracker FreeRTOS ===");
    Serial.println("Registros da inicialização:");

    // 2. Inicializar Hardware (Servos)
    servo1.attach(SERVO_1_PIN);
    servo2.attach(SERVO_2_PIN);
    servo1.write(90); // Posição inicial
    servo2.write(90);
    Serial.println("[MAIN] Drivers de Servo (ESP32Servo) inicializados.");

    // 3. Criar Filas
    q_sensor_data = xQueueCreate(5, sizeof(SensorData));
    q_servo_cmd = xQueueCreate(5, sizeof(ControlOutput));

    if (!q_sensor_data || !q_servo_cmd) {
        Serial.println("[MAIN] !!! FALHA ao criar filas. Travando.");
        while(1) { vTaskDelay(1000); }
    }
    Serial.println("[MAIN] Filas FreeRTOS (Queues) criadas.");

    // 4. Criar Tarefas
    // Prioridades: CTRL(3) > IO(2) > ACTUATOR(2)
    xTaskCreatePinnedToCore(taskIO, "IO_Task", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskControl, "Control_Task", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(taskActuator, "Actuator_Task", 4096, NULL, 2, NULL, 0);

    Serial.println("[MAIN] Tarefas FreeRTOS (Tasks) criadas. Sistema operacional.");
    Serial.println("--------------------------------------------------");
}

void loop() {
    // O loop() fica ocioso. O FreeRTOS está no comando.
    vTaskDelay(pdMS_TO_TICKS(10000));
}