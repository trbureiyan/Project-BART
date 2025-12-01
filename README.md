# <img width="64" height="64" alt="BART_Project_Icon" src="https://github.com/user-attachments/assets/5673729d-81f5-48a7-8320-8e33f82a0819" /> B.A.R.T. (Biometric Alcohol Real-time Tracker)

![Status](https://img.shields.io/badge/Status-Finalizado-success)
![Platform](https://img.shields.io/badge/PLATFORM-ARDUINO%20UNO-blue)
![App](https://img.shields.io/badge/APP-MIT%20APP%20INVENTOR-orange)
![Version](https://img.shields.io/badge/Version-1.0.0-blue)

> **Sistema IoT de telemetría biométrica para la detección y monitoreo de alcohol en tiempo real.**

![BART Device](https://github.com/user-attachments/assets/163b06b7-8476-43b3-b9e7-c2836004b163)
> Designed by @trbureiyan

---

## 📋 Tabla de Contenidos

- [Descripción del Proyecto](#-descripción-del-proyecto)
- [Características Principales](#-características-principales)
- [Arquitectura del Sistema](#️-arquitectura-del-sistema)
- [Documentación Técnica](#-documentación-técnica)
- [Autores](#-autores)

---

## 🎯 Descripción del Proyecto

**B.A.R.T.** es un dispositivo *wearable* prototipo diseñado para la prevención de accidentes y el monitoreo de salud en contextos laborales y educativos. A diferencia de los alcoholímetros analógicos tradicionales, BART opera como un **Sistema de Adquisición de Datos (DAQ)** conectado.

El sistema captura la concentración de alcohol en el aliento mediante un sensor electroquímico MQ-3, procesa la señal digitalmente eliminando ruido ambiental y transmite los datos vía Bluetooth a una aplicación móvil Android, ofreciendo una interfaz visual semántica (tipo semáforo) para la toma de decisiones informadas.

### 🎓 Contexto Académico

Proyecto final desarrollado para la asignatura de **Computación Física** en la **Universidad Surcolombiana** (2025), demostrando la aplicación práctica de sistemas embebidos, sensórica química y desarrollo de aplicaciones IoT de bajo costo.

### 🔬 Objetivos del Proyecto

- **Portabilidad:** Funcionamiento autónomo con visualización local mediante barra de 8 LEDs
- **Conectividad:** Telemetría en tiempo real hacia dispositivos móviles vía Bluetooth Classic
- **Precisión:** Implementación de algoritmos de autocalibración ambiental y filtrado digital
- **Accesibilidad:** Solución de bajo costo (<$120,000 COP) para programas de SST en PyMEs
- **Educación:** Herramienta de sensibilización sobre consumo responsable de alcohol

---

## ✨ Características Principales

### Hardware (Dispositivo Físico)

<img width="500" height="428" alt="BART Core" src="https://github.com/user-attachments/assets/ce8be6cd-e959-49df-a084-712cd50311be" /> <img width="500" height="428" alt="technical art" src="https://github.com/user-attachments/assets/639f2e0d-cc2e-4939-8d99-f932d614395b" />


* **🎯 Autocalibración de Punto Cero:** Al iniciar, el sistema muestrea el aire ambiente durante 5 segundos para establecer una línea base dinámica, compensando cambios de humedad, temperatura y altitud

* **🔊 Procesamiento Digital de Señales (DSP):** Implementación de filtros de promedio móvil (oversampling de 10 muestras) para estabilizar la lectura analógica del sensor MQ-3 y reducir ruido

* **🔌 Arquitectura de Puertos Separados:** Uso de `SoftwareSerial` en pines 2/4 para segregar el canal de telemetría (Bluetooth) del canal de depuración (USB), permitiendo debugging simultáneo durante desarrollo

* **💡 Feedback Visual Dinámico:** Sistema de barra de 8 LEDs (3 verdes, 3 amarillos, 2 rojos) con animaciones de estado:
  - **Knight Rider** durante encendido
  - **Heartbeat** en primer LED verde para indicar sistema activo
  - **Flash completo** de confirmación al guardar medición

* **🔋 Alimentación Portátil:** Pack de 6 pilas AA (9V → 5V regulado) con autonomía de ~10 horas de operación continua o varias semanas en uso intermitente

### Software (Aplicación Móvil)

* **📱 Interfaz Semáforo Intuitiva:** Clasificación visual en 3 niveles con recomendaciones claras:
  - 🟢 **VERDE (Sobrio):** Niveles 1-3 → "Apto para trabajar"
  - 🟡 **AMARILLO (Precaución):** Niveles 4-6 → "Evaluar con equipo certificado"
  - 🔴 **ROJO (Riesgo Alto):** Niveles 7-8 → "No apto para actividades de riesgo"

* **📊 Registro Histórico con Timestamps:** Almacenamiento local (TinyDB) de todas las mediciones con fecha, hora y nivel detectado para análisis de tendencias

* **👤 Sistema Multi-Usuario:** Gestión de perfiles mediante Firebase Authentication, permitiendo uso compartido del dispositivo con historial individual

* **📈 Gráficas en Tiempo Real:** Visualización dinámica de la evolución del nivel de alcohol durante la medición

* **🔐 Privacidad por Diseño:** Datos almacenados localmente en el dispositivo móvil sin transmisión a servidores externos (cumplimiento Ley 1581/2012)

### Protocolo de Comunicación

* **📡 Bluetooth SPP (Serial Port Profile):** Canal bidireccional a 9600 baudios
  - **Arduino → App:** Transmisión de niveles (1, 2, 3) cada 1 segundo con delimitador `\n`
  - **App → Arduino:** Comandos de control (`I`: Iniciar, `X`: Detener, `S`: Guardar)

* **🛡️ Tolerancia a Errores:** Validación de integridad de datos en App Inventor con manejo de paquetes corruptos

---

## 🏗️ Arquitectura del Sistema

El sistema B.A.R.T. implementa una **arquitectura de tres capas** que separa responsabilidades:

```
┌─────────────────────────────────────────────────────────┐
│  CAPA 3: INTERFAZ DE USUARIO (Aplicación Móvil)       │
│  - Visualización semántica (Semáforo)                  │
│  - Almacenamiento local (TinyDB + Firebase)            │
│  - Control remoto (Comandos Bluetooth)                 │
└────────────────┬────────────────────────────────────────┘
                 │ Bluetooth Classic (HC-05)
                 │ 9600 bps, SPP Profile
┌────────────────▼────────────────────────────────────────┐
│  CAPA 2: PROCESAMIENTO Y CONTROL (Arduino UNO)         │
│  - Autocalibración dinámica                            │
│  - Filtrado digital (promedio móvil)                   │
│  - Mapeo de niveles físicos (0-8) → lógicos (1-3)     │
│  - Gestión de comandos y actuadores                    │
└────────────────┬────────────────────────────────────────┘
                 │ ADC 10-bit (Pin A0)
                 │ Señal analógica 0-5V
┌────────────────▼────────────────────────────────────────┐
│  CAPA 1: SENSÓRICA (Sensor MQ-3)                       │
│  - Detección de etanol en aire expirado                │
│  - Elemento SnO₂ calentado a ~350°C                    │
│  - Rango: 25-500 ppm                                   │
│  - Salida: Resistencia variable → Voltaje analógico    │
└─────────────────────────────────────────────────────────┘
```

<img width="640" height="478" alt="BART with phone" src="https://github.com/user-attachments/assets/2f91883e-f948-4453-b33c-52d4bd75273c" />

### Flujo de Datos

1. **Adquisición:** Sensor MQ-3 detecta alcohol → Cambio de resistencia → Divisor de voltaje
2. **Digitalización:** ADC de Arduino convierte voltaje analógico (Pin A0) → Valor digital 0-1023
3. **Procesamiento:** Autocalibración + Filtrado + Mapeo → Nivel 1-3
4. **Transmisión:** Bluetooth HC-05 envía nivel a app cada 1 segundo
5. **Visualización:** App actualiza semáforo y gráfica en tiempo real
6. **Persistencia:** Usuario guarda medición → TinyDB local + Firebase (con timestamp)

---

## 🛠️ Componentes Requeridos



### Software (Stack Tecnológico)

| Componente | Tecnología | Versión | Licencia |
|------------|------------|---------|----------|
| **IDE de Desarrollo** | Arduino IDE | 2.3.2 | GPL |
| **Desarrollo Móvil** | MIT App Inventor 2 | Cloud-based | Apache 2.0 |
| **Backend (Auth/DB)** | Firebase (Auth + Realtime DB) | Latest | Freemium |
| **Control de Versiones** | Git + GitHub | Latest | GPL |
| **Documentación** | Markdown + Excalidraw | - | Open Source |

### Herramientas de Desarrollo

- **Hardware:** Multímetro digital, soldador (opcional), cortafrio de precisión
- **Software:** 
  - Arduino IDE para firmware
  - MIT App Inventor (navegador Chrome recomendado)
  - App Companion para pruebas en dispositivo real
  - Git para control de versiones

---

## 📥 Instalación y Configuración

### Paso 1: Configuración del Hardware

#### 1.1. Montaje del Circuito en Protoboard

El MQ-3 de 6 pines discreto tiene dos circuitos independientes:
- **Heater (H-H):** Pines centrales → Conectar a 5V y GND directamente
- **Sensor (A-B):** Pines externos → Configurar divisor de voltaje

**Conexiones correctas:**
```
Pin A (esquina izquierda superior) → 5V Arduino
Pin B (esquina izquierda inferior) → Resistencia 10kΩ → GND
                                  └→ Pin A0 Arduino
Pines H (4 centrales) → 2 a 5V, 2 a GND (cualquier combinación)
```

#### 1.2. Conexión del Módulo Bluetooth HC-05

```
HC-05 VCC  → Arduino 5V
HC-05 GND  → Arduino GND
HC-05 TX   → Arduino Pin 2 (RX en software)
HC-05 RX   → Arduino Pin 4 (TX en software)
```

#### 1.3. Conexión de Barra de LEDs

```
LED1 (Verde)    Pin 5  ──[220Ω]── GND
LED2 (Verde)    Pin 6  ──[220Ω]── GND
LED3 (Verde)    Pin 7  ──[220Ω]── GND
LED4 (Amarillo) Pin 8  ──[220Ω]── GND
LED5 (Amarillo) Pin 9  ──[220Ω]── GND
LED6 (Amarillo) Pin 10 ──[220Ω]── GND
LED7 (Rojo)     Pin 11 ──[220Ω]── GND
LED8 (Rojo)     Pin 12 ──[220Ω]── GND
```

#### 1.4. Alimentación

```
Pack 6xAA (+) → Arduino VIN
Pack 6xAA (-) → Arduino GND
```

#### 2. Cargar el Código

1. Conectar Arduino UNO al PC vía USB
2. **Desconectar el módulo Bluetooth HC-05** temporalmente (conflicto con puerto serial durante carga)
3. Click en botón **"Subir"** (flecha derecha) en Arduino IDE
4. Esperar mensaje: `"Subida completada"`
5. Reconectar el módulo Bluetooth HC-05

#### 2.1 Verificar Funcionamiento

Al encender el Arduino, deberías ver:
1. Animación tipo "Knight Rider" en los LEDs (barrido izquierda-derecha)
2. Flash completo de todos los LEDs
3. Primer LED verde queda encendido permanentemente (heartbeat)
4. LED integrado Pin 13 parpadea brevemente

### Paso 3: Instalación de la Aplicación Móvil

#### Opción A: Instalar APK Pre-compilada (Recomendado)

1. Descargar la última versión desde [Releases](https://github.com/trbureiyan/Project-BART/releases)
2. En el smartphone Android, habilitar instalación de fuentes desconocidas:
   ```
   Configuración > Seguridad > Fuentes desconocidas > Activar
   ```
3. Abrir el archivo `BART_v1.0.0.apk` descargado
4. Seguir las instrucciones de instalación
5. Aceptar permisos de Bluetooth cuando la app lo solicite

#### Opción B: Compilar desde Código Fuente (Desarrolladores)

1. Acceder a [MIT App Inventor](http://ai2.appinventor.mit.edu)
2. Iniciar sesión con cuenta de Google
3. Ir a `Proyectos > Importar proyecto (.aia) desde mi computadora`
4. Seleccionar el archivo `BART_App.aia` del repositorio
5. Una vez importado, ir a `Generar > Android App (.apk)`
6. Descargar el APK generado e instalar en el dispositivo

---

## 🚀 Uso del Sistema

<img width="640" height="478" alt="BART leds" src="https://github.com/user-attachments/assets/990607a6-0136-4747-af4c-2764fd8c9bd4" />

### Encendido y Calibración Inicial

1. **Conectar el pack de pilas AA** al Arduino (o conectar vía USB)
2. Esperar **30 segundos** para precalentamiento del sensor MQ-3
3. Durante este tiempo, el sistema ejecuta:
   - Calentamiento del elemento SnO₂ a ~350°C
   - Autocalibración con 20 muestras de aire limpio
   - Cálculo de línea base promedio
4. Confirmación visual: Flash completo de LEDs + LED verde permanente

### Conexión Bluetooth

1. **En el smartphone:**
   - Ir a `Configuración > Bluetooth > Emparejar nuevo dispositivo`
   - Buscar dispositivo **"HC-05"** o **"BART-BT"**
   - Emparejar usando PIN: `1234` o `0000`

2. **En la aplicación B.A.R.T.:**
   - Abrir app → Iniciar sesión (o registrarse)
   - En pantalla principal, presionar botón **"Conectar Bluetooth"**
   - Seleccionar "HC-05" de la lista de dispositivos emparejados
   - Verificar mensaje: "Conectado exitosamente"

### Realizar una Medición

1. **Iniciar medición:**
   - Presionar botón **"Iniciar"** en la app
   - El sistema ejecuta nueva calibración de 10 muestras (5 segundos)
   - El primer LED verde comienza a parpadear (sistema activo)

2. **Soplar en el sensor:**
   - Acercar la boca a **5-10 cm** de la rejilla superior del dispositivo
   - Exhalar aire de forma continua durante **3-5 segundos**
   - Observar la barra de LEDs y el semáforo en la app

3. **Interpretar resultados:**
   - 🟢 **Verde (Niveles 1-3):** Sobrio - Apto para actividades normales
   - 🟡 **Amarillo (Niveles 4-6):** Precaución - Evaluar con equipo certificado
   - 🔴 **Rojo (Niveles 7-8):** Riesgo Alto - No apto para conducir ni operar maquinaria

4. **Guardar medición (opcional):**
   - Presionar botón **"Guardar"** en la app
   - Confirmar el guardado con flash de LEDs en el dispositivo
   - El registro queda almacenado con fecha y hora

5. **Detener medición:**
   - Presionar botón **"Detener"** en la app
   - Los LEDs se apagarán excepto el primero (heartbeat)

### Consultar Historial

1. En la app, ir a la sección **"Historial"**
2. Ver lista de mediciones previas con:
   - Fecha y hora de la prueba
   - Nivel detectado (1-3)
   - Clasificación (Sobrio/Precaución/Riesgo)
3. Calcular promedio de mediciones con botón **"Calcular Promedio"**
4. Exportar datos (función futura)

### Apagado del Sistema

**Opción A: Apagado suave**
- Desconectar Bluetooth desde la app
- El Arduino continúa funcionando (medición local con LEDs)
- Desconectar el pack de pilas cuando no se use

**Opción B: Apagado completo**
- Desconectar el pack de pilas del Arduino
- Cerrar sesión en la app móvil

---

## 📚 Documentación Técnica

### Especificaciones del Sensor MQ-3

- **Principio de operación:** Semiconductor de óxido metálico (SnO₂)
- **Rango de detección:** 25-500 ppm de etanol
- **Consumo del heater:** 0.9W (~150 mA a 5V)
- **Tiempo de respuesta:** <10 segundos
- **Tiempo de recuperación:** <90 segundos
- **Sensibilidad:** Rs(aire) / Rs(0.4 mg/L alcohol) ≥ 5

### Algoritmo de Autocalibración

```cpp
// Pseudocódigo del algoritmo
int baseAireLimpio = 0;

void calibrar() {
    int suma = 0;
    for (int i = 0; i < 20; i++) {
        suma += leerPromedioSensor(10); // 10 submuestras por lectura
        delay(200); // 200ms entre lecturas
    }
    baseAireLimpio = suma / 20; // Promedio de 20 lecturas
}
```

### Mapeo de Niveles

**Físico (LEDs) → Lógico (App):**
```
Niveles 0-1 (0-2 LEDs)   → App Nivel 1 (Verde)
Niveles 2-3 (3-4 LEDs)   → App Nivel 1 (Verde)
Niveles 4-5 (5-6 LEDs)   → App Nivel 2 (Amarillo)
Niveles 6   (7 LEDs)     → App Nivel 3 (Rojo)
Niveles 7-8 (8 LEDs)     → App Nivel 3 (Rojo)
```

### Protocolo Serial Bluetooth

**Formato de mensaje Arduino → App:**
```
[NIVEL]\n

Ejemplo: "2\n" (Nivel 2, Amarillo)
```

**Formato de comando App → Arduino:**
```
[COMANDO]

Comandos soportados:
- 'I' o '1': Iniciar medición
- 'X' o '2': Detener medición
- 'S': Guardar medición (feedback visual)
```

---


## 👥 Autores

Desarrollado como proyecto final para la asignatura de **Computación Física**.

* **Brayan Toro Bustos** - *Ingeniería de Software*
* **Pablo Trujillo Artunduaga** - *Ingeniería de Software*

**Institución:** Universidad Surcolombiana

⚠️ DESCARGO DE RESPONSABILIDAD LEGAL

Este dispositivo es un PROTOTIPO EDUCATIVO y NO constituye un instrumento de medición legal según NTC-ISO/IEC 17025.

- NO utilizar como evidencia para despidos laborales
- NO reemplaza alcoholímetros homologados (ej: Dräger 7510)
- Uso recomendado: PRE-SCREENING educativo o autocontrol preventivo
- La decisión final debe basarse en equipos certificados

Cumplimiento Resolución 1843/2025: Este dispositivo puede usarse como herramienta COMPLEMENTARIA de sensibilización, pero los resultados positivos deben confirmarse con equipos certificados.
