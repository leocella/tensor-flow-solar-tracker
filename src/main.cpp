#include <Arduino.h>

void setup() {
  // Use SerialUSB when available (some cores expose SerialUSB for native USB CDC)
#ifdef SerialUSB
  Stream &console = SerialUSB;
#else
  Stream &console = Serial;
#endif

  console.begin(115200);

  // Aguarda alguns instantes para a serial/USB enumerar (máx 3s)
  unsigned long start = millis();
  while (!console && millis() - start < 3000) {
    delay(10);
  }

  // Inicializa LED embutido para confirmar execução mesmo sem serial
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  console.println("\n\n");
  console.println("========================================");
  console.println("    TESTE DE PORTA SERIAL ESP32-S3     ");
  console.println("========================================");
  console.println();
  console.println("Se voce esta vendo esta mensagem,");
  console.println("a comunicacao serial esta funcionando!");
  console.println();
  console.println("O contador abaixo sera atualizado a cada segundo:");
  console.println("========================================");
  console.println();
}

void loop() {
  static int contador = 0;
  static unsigned long ultimoTempo = 0;
  
  // Atualiza a cada 1 segundo
  if (millis() - ultimoTempo >= 1000) {
    ultimoTempo = millis();
    contador++;
    // Mostra diferentes tipos de mensagens para testar bem
#ifdef SerialUSB
    Stream &console = SerialUSB;
#else
    Stream &console = Serial;
#endif

    console.print(">>> Contador: ");
    console.print(contador);
    console.print(" | Tempo: ");
    console.print(millis() / 1000);
    console.println(" segundos");
    
    // A cada 5 segundos mostra uma mensagem especial
    if (contador % 5 == 0) {
      console.println("    *** TESTE OK - Serial funcionando perfeitamente! ***");
    }
    
    // A cada 10 segundos mostra estatísticas
    if (contador % 10 == 0) {
      console.println();
      console.println("----------------------------------------");
      console.println("RESUMO:");
      console.print("  - Mensagens enviadas: ");
      console.println(contador);
      console.print("  - Tempo total: ");
      console.print(millis() / 1000);
      console.println(" segundos");
      console.print("  - Memoria livre: ");
      console.print(ESP.getFreeHeap());
      console.println(" bytes");
      console.println("----------------------------------------");
      console.println();
    }

    // Toggle LED so user sees activity even if serial is not connected
    digitalWrite(LED_BUILTIN, (contador % 2) ? HIGH : LOW);
  }
}
