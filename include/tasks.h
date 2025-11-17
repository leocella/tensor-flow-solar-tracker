// include/tasks.h
#pragma once
#include <Arduino.h>

// Dados crus dos sensores (IO -> Control)
struct SensorData {
    int ldrTL, ldrTR, ldrBL, ldrBR;
    float vPanel; // TODO: Adicionar leitura da tensão
};

// Comando de atuação (Control -> Actuator)
struct ControlOutput {
    int targetPos1; // Alvo para servo 1
    int targetPos2; // Alvo para servo 2
    bool useML;     // Flag para log
};

// Protótipos das tarefas
void taskIO(void* arg);
void taskControl(void* arg);
void taskActuator(void* arg);
// void taskMQTT(void* arg); // (futuro)