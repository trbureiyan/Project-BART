/*
 * ALCOHOLÍMETRO FINAL - ARQUITECTURA SEPARADA
 * * CORRECCIÓN CRÍTICA:
 * - Se usa SoftwareSerial en pines 10 y 11 para no chocar con el USB.
 * - El Monitor Serie (PC) SOLO sirve para dar órdenes (Menú).
 * - El Bluetooth (App) recibe SOLO datos numéricos del sensor.
 * * CABLEADO:
 * - HC-05 TX  -> Arduino Pin 10
 * - HC-05 RX  -> Arduino Pin 11
 * - Sensor    -> A0
 */

#include <SoftwareSerial.h>
#include <EEPROM.h>

// --- PINES ---
// ¡IMPORTANTE! NO USAR 0 y 1. Usamos 10 y 11.
const int PIN_RX_BT = 2;  // Aquí entra el cable TX del Módulo BT
const int PIN_TX_BT = 4;  // Aquí entra el cable RX del Módulo BT
const int PIN_SENSOR = A0;
const int PIN_LED_TESTIGO = 13;

int ledPins[] = {5, 6, 7, 8, 9, 10, 11, 12}; 
const int LED_COUNT = 8;

// --- OBJETOS ---
SoftwareSerial BTSerial(PIN_RX_BT, PIN_TX_BT);

// --- VARIABLES ---
bool pruebaActiva = false;    // Controla si enviamos datos o no
unsigned long lastUpdate = 0;

// Calibración (Ajustada para ver cambios reales)
const int SENSOR_MIN = 20;    // Valor mínimo de ruido
const int SENSOR_MAX = 600;   // Valor máximo esperado

void setup() {
  // 1. Puerto Serie PC (Solo Debug)
  Serial.begin(9600);
  
  // 2. Puerto Serie Bluetooth (Comunicación App)
  BTSerial.begin(9600);

  // 3. Configurar Pines
  pinMode(PIN_LED_TESTIGO, OUTPUT);
  for (int i = 0; i < LED_COUNT; i++) pinMode(ledPins[i], OUTPUT);

  Serial.println("\n--- SISTEMA LISTO Y SEPARADO ---");
  Serial.println("CONEXION:");
  Serial.println("  * Bluetooth TX conectado al Pin 10");
  Serial.println("  * Bluetooth RX conectado al Pin 11");
  Serial.println("MENU PC (Escribe el numero y dale Enter):");
  Serial.println("  [1] FORZAR INICIO (Simular que App envio 'I')");
  Serial.println("  [2] FORZAR PARADA (Simular que App envio 'X')");
  Serial.println("----------------------------------------------");
}

void loop() {
  // ==========================================
  // A. LEER COMANDOS DEL PC (SOLO MENU)
  // ==========================================
  if (Serial.available()) {
    char cmdPC = Serial.read();
    // Limpiamos saltos de línea o espacios
    if(cmdPC == '\n' || cmdPC == '\r' || cmdPC == ' ') return;

    if (cmdPC == '1') {
      pruebaActiva = true;
      Serial.println("[PC] -> COMANDO FORZADO: INICIANDO TRANSMISION");
    } 
    else if (cmdPC == '2') {
      pruebaActiva = false;
      apagarLeds();
      Serial.println("[PC] -> COMANDO FORZADO: DETENIENDO");
    }
    else {
      Serial.println("[PC] -> Comando no reconocido. Usa 1 o 2.");
    }
    // NOTA: Aquí NO hay ningún "BTSerial.print", por eso no crashea la App.
  }

  // ==========================================
  // B. LEER COMANDOS DE LA APP (BLUETOOTH)
  // ==========================================
  if (BTSerial.available()) {
    char cmdBT = BTSerial.read();
    
    // Feedback visual para saber si llega señal
    digitalWrite(PIN_LED_TESTIGO, HIGH);
    delay(50); 
    digitalWrite(PIN_LED_TESTIGO, LOW);

    // Debug en PC para ver qué llega
    Serial.print("[BLUETOOTH] Llego: ");
    Serial.println(cmdBT);

    if (cmdBT == 'I' || cmdBT == 'i') {
      pruebaActiva = true;
      Serial.println("   -> La App solicito INICIO.");
    } 
    else if (cmdBT == 'X' || cmdBT == 'x') {
      pruebaActiva = false;
      apagarLeds();
      Serial.println("   -> La App solicito DETENER.");
    }
    else if (cmdBT == 'S' || cmdBT == 's') {
      Serial.println("   -> La App solicito GUARDAR.");
      guardarEnEEPROM();
    }
  }

  // ==========================================
  // C. LECTURA Y ENVÍO AUTOMÁTICO
  // ==========================================
  // Se ejecuta cada 300ms si la prueba está activa
  if (pruebaActiva && (millis() - lastUpdate > 300)) {
    lastUpdate = millis();

    // 1. Leer Sensor Real
    int raw = analogRead(PIN_SENSOR);
    
    // 2. Mapear a LEDs (0 a 8)
    int nivelLed = map(raw, SENSOR_MIN, SENSOR_MAX, 0, LED_COUNT);
    nivelLed = constrain(nivelLed, 0, LED_COUNT);

    // 3. Mostrar en Hardware (LEDs físicos)
    mostrarNivelEnLEDs(nivelLed);

    // 4. Enviar DATOS a la App (Solo números)
    // Esto es lo que recibe tu gráfica
    BTSerial.println(nivelLed); 
    
    // 5. Ver en PC qué se está enviando
    Serial.print("Enviando a App -> Sensor RAW: ");
    Serial.print(raw);
    Serial.print(" | Nivel: ");
    Serial.println(nivelLed);
  }
}

// --- FUNCIONES AUXILIARES ---

void mostrarNivelEnLEDs(int nivel) {
  for (int i = 0; i < LED_COUNT; i++) {
    if (i < nivel) digitalWrite(ledPins[i], HIGH);
    else digitalWrite(ledPins[i], LOW);
  }
}

void apagarLeds() {
  for (int i = 0; i < LED_COUNT; i++) digitalWrite(ledPins[i], LOW);
}

void guardarEnEEPROM() {
   // Simulación de guardado para debug
   for(int i=0; i<3; i++) {
     digitalWrite(PIN_LED_TESTIGO, HIGH); delay(50);
     digitalWrite(PIN_LED_TESTIGO, LOW); delay(50);
   }
   Serial.println("   [EEPROM] Guardado simulado ejecutado.");
}
