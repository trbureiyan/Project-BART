/*
 * PROYECTO: ALCOHOLÍMETRO IOT - VERSIÓN PRODUCCIÓN FINAL (v2)
 * AUTORES: Brayan Toro Bustos & Pablo Trujillo Artunduaga
 * UNIVERSIDAD SURCOLOMBIANA - Computación Física
 *
 * DESCRIPCIÓN:
 * Sistema de detección de alcohol con sensor MQ-3 (6 pines) recableado correctamente.
 * - Salida Física: Barra de 8 LEDs progresiva.
 * - Salida Bluetooth: Escala simplificada de 3 niveles (1=Verde, 2=Amarillo, 3=Rojo).
 *
 * MAPEO DE HARDWARE:
 * - Arduino UNO
 * - Sensor MQ-3: A0 (Usando circuito divisor de voltaje externo)
 * - Bluetooth HC-05: TX -> Pin 2, RX -> Pin 4 (SoftwareSerial)
 * - LEDs: Pines 5 a 12
 */

#include <SoftwareSerial.h>
#include <EEPROM.h>

// ================= CONFIGURACIÓN DE PINES =================
const int PIN_RX_BT = 2;    // Conectar TX del Bluetooth aquí
const int PIN_TX_BT = 4;    // Conectar RX del Bluetooth aquí
const int PIN_SENSOR = A0;
const int PIN_LED_TESTIGO = 13; // Indicador de actividad

// ARRAY DE LEDS: ORDEN FÍSICO (Verdes -> Amarillos -> Rojos)
// Índices: 0-2 (Verdes), 3-5 (Amarillos), 6-7 (Rojos)
int ledPins[] = {5, 6, 7, 8, 9, 10, 11, 12}; 
const int LED_COUNT = 8;

// ================= OBJETOS Y VARIABLES =================
SoftwareSerial BTSerial(PIN_RX_BT, PIN_TX_BT);

// Variables de Control
bool pruebaActiva = false;    
unsigned long lastUpdate = 0;
const int INTERVALO_ENVIO = 500; // 500ms entre envíos a la App

// Variables de Calibración
int baseAireLimpio = 0;       // Referencia de aire limpio
const int RANGO_DETECCION = 300; // Sensibilidad (Delta máximo)

void setup() {
  Serial.begin(9600);     // Monitor Serie (PC)
  BTSerial.begin(9600);   // Bluetooth (App)

  // Configurar Pines de Salida
  pinMode(PIN_LED_TESTIGO, OUTPUT);
  for (int i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  Serial.println("=== ALCOHOLIMETRO IOT INICIADO ===");
  
  // --- FASE 1: CALENTAMIENTO Y CALIBRACIÓN ---
  Serial.print("Calibrando aire base (No soplar)...");
  
  long acumulado = 0;
  // Tomamos 20 muestras rápidas para fijar el "Cero"
  for(int i=0; i<20; i++) {
    acumulado += analogRead(PIN_SENSOR);
    // Animación de espera en LEDs
    digitalWrite(ledPins[i % LED_COUNT], HIGH);
    delay(50);
    digitalWrite(ledPins[i % LED_COUNT], LOW);
  }
  
  baseAireLimpio = acumulado / 20;
  Serial.print(" BASE: ");
  Serial.println(baseAireLimpio);
  
  // Señal de LISTO
  digitalWrite(PIN_LED_TESTIGO, HIGH); delay(500); digitalWrite(PIN_LED_TESTIGO, LOW);
  Serial.println("SISTEMA LISTO. Esperando comando 'I'...");
}

void loop() {
  // 1. ESCUCHA COMANDOS (App o PC)
  verificarComandosEntrantes();

  // 2. MEDICIÓN Y PROCESAMIENTO
  if (pruebaActiva && (millis() - lastUpdate > INTERVALO_ENVIO)) {
    lastUpdate = millis();

    // A) LEER SENSOR (Con filtro promedio)
    int lecturaRaw = leerPromedioSensor(10); 
    
    // B) CALCULAR DELTA (Incremento sobre el aire limpio)
    int delta = lecturaRaw - baseAireLimpio;
    if (delta < 0) delta = 0;

    // C) MAPEO FÍSICO (0 a 8 LEDs)
    // Convierte el delta (0-300) a escala de LEDs (0-8)
    int nivelLed = map(delta, 20, RANGO_DETECCION, 0, LED_COUNT);
    nivelLed = constrain(nivelLed, 0, LED_COUNT);

    // D) MAPEO LÓGICO APP (1, 2, 3)
    // Traduce la escala de 8 LEDs a los 3 colores del semáforo
    int nivelApp = 1; // Por defecto Verde (Sobrio)
    
    if (nivelLed <= 3) {
      nivelApp = 1; // Verde (LEDs 0, 1, 2 encendidos o menos)
    } 
    else if (nivelLed > 3 && nivelLed <= 6) {
      nivelApp = 2; // Amarillo (LEDs 3, 4, 5 encendidos)
    } 
    else if (nivelLed > 6) {
      nivelApp = 3; // Rojo (LEDs 6, 7 encendidos)
    }

    // E) ACTUADORES
    mostrarBarraLeds(nivelLed); // Muestra los 8 LEDs progresivos
    enviarDatosApp(nivelApp);   // Envía solo 1, 2 o 3 a la App
    
    // F) DEBUG (Monitor Serie)
    Serial.print("Delta: "); Serial.print(delta);
    Serial.print(" | LEDs: "); Serial.print(nivelLed);
    Serial.print("/8 | App: "); Serial.println(nivelApp);
  }
}

// ================= FUNCIONES AUXILIARES =================

int leerPromedioSensor(int muestras) {
  long suma = 0;
  for(int i=0; i<muestras; i++) {
    suma += analogRead(PIN_SENSOR);
    delay(5);
  }
  return (int)(suma / muestras);
}

void mostrarBarraLeds(int nivel) {
  // Apagar todos
  for (int i = 0; i < LED_COUNT; i++) digitalWrite(ledPins[i], LOW);

  // Encender hasta el nivel actual
  for (int i = 0; i < nivel; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  
  // Opcional: Mantener siempre el primer LED verde tenue como "ON"
  if (nivel == 0 && pruebaActiva) digitalWrite(ledPins[0], HIGH);
}

void enviarDatosApp(int valor) {
  // Envía el número simplificado (1, 2 o 3) seguido de salto de línea
  BTSerial.println(valor);
}

void verificarComandosEntrantes() {
  // Comandos Bluetooth
  if (BTSerial.available()) {
    procesarComando(BTSerial.read(), "BT");
  }
  // Comandos PC
  if (Serial.available()) {
    char c = Serial.read();
    if(c != '\n' && c != '\r') procesarComando(c, "PC");
  }
}

void procesarComando(char cmd, String fuente) {
  if (cmd == 'I' || cmd == 'i' || cmd == '1') {
    pruebaActiva = true;
    Serial.println("-> INICIANDO PRUEBA (" + fuente + ")");
    // Recalibrar base al iniciar para mayor precisión
    baseAireLimpio = leerPromedioSensor(10);
  } 
  else if (cmd == 'X' || cmd == 'x' || cmd == '2') {
    pruebaActiva = false;
    mostrarBarraLeds(0);
    Serial.println("-> PRUEBA DETENIDA (" + fuente + ")");
  }
  else if (cmd == 'S' || cmd == 's') {
    Serial.println("-> GUARDANDO (" + fuente + ")");
    // Feedback visual rápido
    digitalWrite(PIN_LED_TESTIGO, HIGH); delay(100); digitalWrite(PIN_LED_TESTIGO, LOW);
  }
}
