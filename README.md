# <img width="64" height="64" alt="BART_Project_Icon 1" src="https://github.com/user-attachments/assets/5673729d-81f5-48a7-8320-8e33f82a0819" /> B.A.R.T. (Biometric Alcohol Real-time Tracker) 

![Status](https://img.shields.io/badge/Status-Finalizado-success)
![Platform](https://img.shields.io/badge/PLATFORM-ARDUINO%20UNO-blue)
![App](https://img.shields.io/badge/APP-MIT%20APP%20INVENTOR-orange)

> **Sistema IoT de telemetría biométrica para la detección y monitoreo de alcohol en tiempo real.**

---

![unnamed](https://github.com/user-attachments/assets/163b06b7-8476-43b3-b9e7-c2836004b163)
> Designed by @trbureiyan

## Descripción del Proyecto

**BART** es un dispositivo *wearable* prototipo diseñado para la prevención de accidentes y el monitoreo de salud. A diferencia de los alcoholímetros analógicos tradicionales, BART opera como un **Sistema de Adquisición de Datos (DAQ)** conectado.

El sistema captura la concentración de alcohol en el aliento mediante un sensor electroquímico, procesa la señal digitalmente eliminando ruido ambiental y transmite los datos vía Bluetooth a una aplicación móvil Android, ofreciendo una interfaz visual semántica (Semáforo) para la toma de decisiones.

### Objetivos
- **Portabilidad:** Funcionamiento autónomo con visualización local.
- **Conectividad:** Telemetría en tiempo real hacia dispositivos móviles.
- **Precisión:** Implementación de algoritmos de autocalibración ambiental.

---

## Características Técnicas (Highlights)

* **Autocalibración de Punto Cero (Zero-Point Calibration):** Al iniciar, el sistema muestrea el aire ambiente para establecer una línea base dinámica, compensando cambios de humedad y temperatura.
* **Procesamiento Digital de Señales (DSP):** Implementación de filtros de promedio móvil (Oversampling) para estabilizar la lectura analógica del sensor MQ-3.
* **Arquitectura de Puertos Separados:** Uso de `SoftwareSerial` para segregar el canal de telemetría (Bluetooth) del canal de depuración (USB), permitiendo mantenimiento en caliente.
* **Feedback Visual Dinámico:** Animaciones de estado mediante matriz de LEDs (*Knight Rider*, *Heartbeat*, *Flash* de confirmación).
* **Protocolo Robusto:** Comunicación serial asíncrona con delimitadores de línea (`\n`) para garantizar la integridad de los paquetes de datos en la App.

---

## Arquitectura de Hardware

### Lista de Materiales (BOM)
* Microcontrolador: **Arduino UNO** (ATmega328p).
* Sensor: **MQ-3** (Configuración discreta de 6 pines).
* Comunicación: Módulo Bluetooth **HC-05** o **IS-05**.
* Interfaz Local: 8 LEDs (3 Verdes, 3 Amarillos, 2 Rojos).
* Componentes Pasivos: Resistencias de 220Ω (para LEDs) y 10kΩ (Pull-down sensor).

### Diagrama de Conexiones (Pinout)

| Componente | Pin Arduino | Notas Técnicas |
| :--- | :--- | :--- |
| **Sensor MQ-3 (Señal)** | `A0` | Divisor de voltaje con R 10kΩ a GND |
| **Sensor MQ-3 (Heater)**| `5V` / `GND` | Pines medios (H) conectados directo a fuente |
| **Bluetooth TX** | `Pin 2` | Configurado como RX en Arduino (SoftwareSerial) |
| **Bluetooth RX** | `Pin 4` | Configurado como TX en Arduino (SoftwareSerial) |
| **LEDs (Verdes)** | `5`, `6`, `7` | Indicadores de estado SOBRIO |
| **LEDs (Amarillos)** | `8`, `9`, `10` | Indicadores de estado PRECAUCIÓN |
| **LEDs (Rojos)** | `11`, `12` | Indicadores de estado EBRIO |
| **Testigo Sistema** | `13` | LED integrado para feedback de transmisión |

---

## Arquitectura de Software

### Firmware (Arduino)
El código (`Alcoholimetro_BART_Ultimate.ino`) maneja la lógica de control bajo una arquitectura no bloqueante basada en `millis()`.
- **Mapeo Físico:** Convierte la lectura delta (0-350) a una escala de 8 LEDs.
- **Mapeo Lógico:** Traduce la escala física a 3 niveles para la App (1, 2, 3).

### Aplicación Móvil (MIT App Inventor)
La interfaz actúa como cliente Bluetooth SPP.
- **Lógica de Semáforo:**
    - `Nivel <= 3` 🟢 **VERDE** (Zona Segura)
    - `3 < Nivel <= 6` 🟡 **AMARILLO** (Precaución)
    - `Nivel > 6` 🔴 **ROJO** (Peligro)
- **Configuración Crítica:** `DelimiterByte = 10` para sincronización de tramas.

---

## 👥 Autores y Créditos

Desarrollado como proyecto final para la asignatura de **Computación Física**.

* **Brayan Toro Bustos** - *Ingeniería de Software*
* **Pablo Trujillo Artunduaga** - *Ingeniería de Software*

**Institución:** Universidad Surcolombiana
