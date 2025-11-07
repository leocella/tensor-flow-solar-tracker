#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "SERIAL_TEST";

void app_main(void)
{
    int contador = 0;
    
    printf("\n\n");
    printf("========================================\n");
    printf("    TESTE DE PORTA SERIAL ESP32-S3     \n");
    printf("========================================\n");
    printf("\n");
    printf("Se voce esta vendo esta mensagem,\n");
    printf("a comunicacao serial esta funcionando!\n");
    printf("\n");
    printf("O contador abaixo sera atualizado a cada segundo:\n");
    printf("========================================\n");
    printf("\n");
    
    while(1) {
        contador++;
        
        // Mostra diferentes tipos de mensagens para testar bem
        printf(">>> Contador: %d | Tempo: %d segundos\n", 
               contador, contador);
        
        // A cada 5 segundos mostra uma mensagem especial
        if (contador % 5 == 0) {
            printf("    *** TESTE OK - Serial funcionando perfeitamente! ***\n");
        }
        
        // A cada 10 segundos mostra estatísticas
        if (contador % 10 == 0) {
            printf("\n");
            printf("----------------------------------------\n");
            printf("RESUMO:\n");
            printf("  - Mensagens enviadas: %d\n", contador);
            printf("  - Tempo total: %d segundos\n", contador);
            printf("  - Memoria livre: %d bytes\n", esp_get_free_heap_size());
            printf("----------------------------------------\n");
            printf("\n");
        }
        
        // Aguarda 1 segundo
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
