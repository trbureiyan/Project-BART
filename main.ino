/*
 * # BART (Biometric Alcohol Real-time Tracker)
 * AUTORES: Brayan Toro Bustos Y Pablo Trujillo Artunduaga
 * INSTITUCIÓN: Universidad Surcolombiana - Ingeniería de Software
 * Asignatura: Computacion Fisica
 * Profesor: Diego Andres Carvajal
 * 
 * # DESCRIPCIÓN:
 * Sistema IoT de adquisición de datos biométricos (alcohol en aliento).
 * Utiliza un sensor MQ-3 en configuración de divisor de voltaje y un módulo
 * Bluetooth HC-05 para telemetría móvil.
 * 
 * # MAPEO DE HARDWARE (ARDUINO UNO):
 * 
 * ## [ENTRADAS]
 * - Sensor MQ-3 (Señal):   Pin A0
 * - Bluetooth RX (Dato):   Pin 2 (Configurado como RX en Arduino)
 * 
 * ## [SALIDAS]
 * - Bluetooth TX (Dato):   Pin 4 (Configurado como TX en Arduino)
 * - Barra LEDs (Visual):   Pines 5, 6, 7 (Verdes), 8, 9, 10 (Amarillos), 11, 12 (Rojos)
 * - Testigo Sistema:       Pin 13 (Led integrado)
 *
 * ## [MQ-3]
 * - Heater (H):            Conectado directo a 5V y GND
 * - Circuito A-B:          5V -> [Sensor] -> A0 -> [10k Ohm] -> GND
 * ----------------------------------------------------------------------------------------- |
 */

#include <SoftwareSerial.h>

// --- Pines Config ---
const int PIN_RX_BT = 2;    // TX del Bluetooth
const int PIN_TX_BT = 4;    // RX del Bluetooth
const int PIN_SENSOR = A0;
const int PIN_LED_TESTIGO = 13;

// Arreglo de LEDs: Orden físico de Izquierda (Verde), Centro (Amarillo) y a Derecha (Rojo)
int ledPins[] = {5, 6, 7, 8, 9, 10, 11, 12}; 
const int LED_COUNT = 8;

// --- Obj ---
SoftwareSerial BTSerial(PIN_RX_BT, PIN_TX_BT);

// --- Env Vars ---
bool pruebaActiva = false;     // Estado principal
unsigned long lastUpdate = 0;
const int INTERVALO_ENVIO = 1000; // 1000ms

// --- CALIBRACIÓN Vars ---
int baseAireLimpio = 0;        // Valor ambiente
const int RANGO_DETECCION = 350; // Sensibilidad (Delta max)

void setup() {
  // 1. Inicialización de Puertos
  Serial.begin(9600);
  BTSerial.begin(9600);   // Telemetría

  // 2. Configuración de Salidas
  pinMode(PIN_LED_TESTIGO, OUTPUT);
  for (int i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  Serial.println(F("\n--- INICIANDO SISTEMA BART v3.0 ---"));
  
  // 3. Animación de Encendido (Test de Hardware)
  Serial.println(F("[ESTADO] Verificando LEDs..."));
  animacionKnightRider(2); // Hace un barrido visual

  // 4. Precalentamiento y Calibración
  Serial.print(F("[ESTADO] Calibrando sensor (Aire limpio)..."));
  long acumulado = 0;
  
  // Tomamos 20 muestras para estabilizar la línea base
  for(int i=0; i<20; i++) {
    acumulado += analogRead(PIN_SENSOR);
    // Feedback de carga en el LED testigo
    digitalWrite(PIN_LED_TESTIGO, !digitalRead(PIN_LED_TESTIGO));
    delay(100);
  }
  digitalWrite(PIN_LED_TESTIGO, LOW);

  baseAireLimpio = acumulado / 20;
  Serial.print(F(" BASE FIJADA: "));
  Serial.println(baseAireLimpio);
  
  // Animación de "Listo" (3 parpadeos rápidos verdes)
  animacionListo();
  Serial.println(F("[ESTADO] Sistema en Espera. Escuchando Bluetooth..."));
}

void loop() {
  // ============================================================
  // 1. CAPA DE ESCUCHA
  // ============================================================
  verificarComandosEntrantes();

  // ============================================================
  // 2. CAPA DE PROCESAMIENTO (SOLO SI ESTÁ ACTIVO)
  // ============================================================
  if (pruebaActiva && (millis() - lastUpdate > INTERVALO_ENVIO)) {
    lastUpdate = millis();

    // 2.1 Adquisición de Datos (Filtro Promedio)
    int lecturaRaw = leerPromedioSensor(10); 
    
    // 2.2 Cálculo Diferencial (Delta)
    int delta = lecturaRaw - baseAireLimpio;
    if (delta < 0) delta = 0;

    // 2.3 Mapeo Físico (Hardware 0-8 LEDs)
    // Conversion magnitud química (0-350) a visual (0-8)
    int nivelLed;
    if (delta < 20) {
      nivelLed = 0; // Zona muerta para evitar ruido
    } else if (delta > RANGO_DETECCION) {
      nivelLed = LED_COUNT; // Saturación
      // Enviar alerta especial a la app
    } else {
      nivelLed = map(delta, 20, RANGO_DETECCION, 0, LED_COUNT);
    }

    // 2.4 Mapeo Lógico (Software App 1-3)
    // Categorización semántica para el usuario final
    int nivelApp = 1; // Default
    
    if (nivelLed <= 3) {
      nivelApp = 1; // Zona Segura (Verde)
    } 
    else if (nivelLed > 3 && nivelLed <= 6) {
      nivelApp = 2; // Zona Precaución (Amarillo)
    } 
    else if (nivelLed > 6) {
      nivelApp = 3; // Zona Peligro (Rojo)
    }

    // 2.5 Actualización de Salidas
    mostrarBarraLeds(nivelLed); // barra física
    enviarDatosApp(nivelApp);   // dato a móvil
    
    // 2.6 Logs (Debug)
    Serial.print("Delta: "); Serial.print(delta);
    Serial.print(" | Hardware: "); Serial.print(nivelLed);
    Serial.print("/8 | App: "); Serial.println(nivelApp);
  }
}

// FUNCIONES AUXILIARES Y ANIMACIONES

// --- ANIMACIÓN DE INICIO (KNIGHT RIDER) ---
void animacionKnightRider(int ciclos) {
  for(int c=0; c<ciclos; c++) {
    // Ida
    for (int i = 0; i < LED_COUNT; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(30);
      digitalWrite(ledPins[i], LOW);
    }
    // Vuelta
    for (int i = LED_COUNT - 1; i >= 0; i--) {
      digitalWrite(ledPins[i], HIGH);
      delay(30);
      digitalWrite(ledPins[i], LOW);
    }
  }
}

// --- ANIMACIÓN DE LISTO ---
void animacionListo() {
  // Parpadea los primeros 3 LEDs (Verdes)
  for(int k=0; k<3; k++){
    for(int i=0; i<3; i++) digitalWrite(ledPins[i], HIGH);
    delay(100);
    for(int i=0; i<3; i++) digitalWrite(ledPins[i], LOW);
    delay(100);
  }
}

// --- ANIMACIÓN DE COMANDO RECIBIDO ---
void animacionFeedback() {
  // Parpadeo rápido del LED integrado (Testigo)
  digitalWrite(PIN_LED_TESTIGO, HIGH);
  delay(50);
  digitalWrite(PIN_LED_TESTIGO, LOW);
}

// --- LECTURA DE SENSOR ---
int leerPromedioSensor(int muestras) {
  long suma = 0;
  for(int i=0; i<muestras; i++) {
    suma += analogRead(PIN_SENSOR);
    delay(4); // Pequeño delay para permitir recuperación del ADC
  }
  return (int)(suma / muestras);
}

// --- CONTROL DE BARRA DE LEDS ---
void mostrarBarraLeds(int nivel) {
  // Apagar toda la barra
  for (int i = 0; i < LED_COUNT; i++) digitalWrite(ledPins[i], LOW);

  // Llenar hasta el nivel
  for (int i = 0; i < nivel; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  
  // "Latido" del sistema: Si está activo pero en 0,
  // mantenemos el primer LED verde muy tenue (parpadeo lento simulado por código)
  // para que el usuario sepa que está midiendo.
  if (nivel == 0 && pruebaActiva) {
     // Simple encendido fijo del primer led para indicar "ON"
     digitalWrite(ledPins[0], HIGH); 
  }
}

// --- TRANSMISIÓN BLUETOOTH ---
void enviarDatosApp(int valor) {
  BTSerial.println(valor);
  // Feedback visual de transmisión en el pin 13
  digitalWrite(PIN_LED_TESTIGO, HIGH);
  delay(20); // Muy breve para no bloquear
  digitalWrite(PIN_LED_TESTIGO, LOW);
}

// --- GESTOR DE COMANDOS ---
void verificarComandosEntrantes() {
  // 1. Revisar Bluetooth
  if (BTSerial.available()) {
    procesarComando(BTSerial.read(), "BT");
  }
  // 2. Revisar Monitor Serie (PC)
  if (Serial.available()) {
    char c = Serial.read();
    if(c != '\n' && c != '\r') procesarComando(c, "PC");
  }
}

void procesarComando(char cmd, String fuente) {
  animacionFeedback(); // Feedback visual inmediato de que llegó algo

  if (cmd == 'I' || cmd == 'i' || cmd == '1') {
    pruebaActiva = true;
    Serial.println("[CMD] INICIO RECIBIDO (" + fuente + ")");
    // Recalibración rápida
    baseAireLimpio = leerPromedioSensor(10);
    animacionListo(); // Confirma visualmente el inicio
  } 
  else if (cmd == 'X' || cmd == 'x' || cmd == '2') {
    pruebaActiva = false;
    mostrarBarraLeds(0); // Apagar barra
    Serial.println("[CMD] DETENCION (" + fuente + ")");
    // Animación de apagado (barrido inverso)
    for (int i = LED_COUNT - 1; i >= 0; i--) {
      digitalWrite(ledPins[i], HIGH); delay(20); digitalWrite(ledPins[i], LOW);
    }
  }
  else if (cmd == 'S' || cmd == 's') {
     Serial.println("[CMD] GUARDAR HISTORIAL (" + fuente + ")");
     // Animación especial de guardado (Flash completo)
     for(int k=0; k<2; k++) {
       for(int i=0; i<LED_COUNT; i++) digitalWrite(ledPins[i], HIGH);
       delay(100);
       for(int i=0; i<LED_COUNT; i++) digitalWrite(ledPins[i], LOW);
       delay(100);
     }
  }
}
