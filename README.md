# 🎨 ESP32 RGB LED Controller

Control de tira LED RGB 2835 (12V) con **Blynk App** y **modo reactivo a música**.

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-orange)
![Blynk](https://img.shields.io/badge/Blynk-IoT-green)

---

## 📦 Lista de Componentes

| Componente | Cantidad | Descripción | Precio Aprox. (CLP) |
|------------|----------|-------------|---------------------|
| ESP32 DevKit V1 | 1 | Microcontrolador WiFi/BT | $5.000 - $8.000 |
| Tira LED RGB 2835 5m | 1 | 300 LEDs, 12V, la que compraste | $8.000 - $12.000 |
| MOSFET IRLZ44N | 3 | Logic-level, TO-220 | $500 c/u |
| Fuente 12V 3A | 1 | Para alimentar LEDs | $3.000 - $5.000 |
| Sensor de Sonido KY-038 | 1 | Micrófono con salida analógica | $1.500 - $2.500 |
| Resistencias 100Ω | 3 | 1/4W | $50 c/u |
| Protoboard | 1 | 830 puntos | $2.000 |
| Cables Dupont | ~20 | Macho-Macho | $1.500 |

**Alternativas de MOSFET:** IRF3205, IRF540N (necesitan 10V en gate, menos ideal), IRLB8721

---

## 🔌 Esquema de Conexiones

### Diagrama General

```
                                    ┌─────────────────────────────────┐
                                    │      FUENTE DE PODER 12V        │
                                    │  ┌─────┐                        │
                                    │  │ AC  │ ══════╗                │
                                    │  │ IN  │       ║ 220V           │
                                    │  └─────┘       ║                │
                                    │  ┌─────┐  ┌────╨────┐           │
                                    │  │+12V │  │  GND    │           │
                                    │  └──┬──┘  └────┬────┘           │
                                    └─────┼──────────┼────────────────┘
                                          │          │
          ┌───────────────────────────────┼──────────┼───────────────────────────┐
          │                               │          │                           │
          │    ╔══════════════════════════╧══════════╧════════════════════╗      │
          │    ║            TIRA LED RGB 2835 - 5 METROS                  ║      │
          │    ║  ┌────────────────────────────────────────────────────┐  ║      │
          │    ║  │  +12V    R (Rojo)    G (Verde)    B (Azul)         │  ║      │
          │    ║  └────┬────────┬────────────┬────────────┬────────────┘  ║      │
          │    ╚═══════╪════════╪════════════╪════════════╪═══════════════╝      │
          │            │        │            │            │                      │
          │     +12V───┘        │            │            │                      │
          │                     │            │            │                      │
          │              ┌──────┴──────┐ ┌───┴────┐ ┌─────┴────┐                 │
          │              │   MOSFET    │ │ MOSFET │ │  MOSFET  │                 │
          │              │   ROJO      │ │ VERDE  │ │   AZUL   │                 │
          │              │  IRLZ44N    │ │IRLZ44N │ │ IRLZ44N  │                 │
          │              │    ___      │ │   ___  │ │    ___   │                 │
          │              │   |   |     │ │  |   | │ │   |   |  │                 │
          │         D────┤   | M |     │ │  | M | │ │   | M |  ├────D            │
          │              │   |___|     │ │  |___| │ │   |___|  │                 │
          │              │  G  │  S    │ │ G │  S │ │  G │  S  │                 │
          │              └──┬──┴──┬────┘ └─┬─┴──┬─┘ └──┬─┴──┬──┘                 │
          │                 │     │        │    │      │    │                    │
          │                 │     │        │    │      │    │                    │
          │              100Ω    GND    100Ω   GND  100Ω   GND                   │
          │                 │     │        │    │      │    │                    │
          │                 │     └────────┴────┴──────┴────┘                    │
          │                 │                   │                                │
          │                 │                   │ (GND común)                    │
          │                 │                   │                                │
          │    ┌────────────┴───────────────────┴────────────────────────┐       │
          │    │                      ESP32                              │       │
          │    │  ┌──────────────────────────────────────────────────┐   │       │
          │    │  │                                                  │   │       │
          │    │  │   GPIO25 ────── 100Ω ──── Gate MOSFET Rojo      │   │       │
          │    │  │   GPIO26 ────── 100Ω ──── Gate MOSFET Verde     │   │       │
          │    │  │   GPIO27 ────── 100Ω ──── Gate MOSFET Azul      │   │       │
          │    │  │   GPIO34 ────── Salida Analógica Sensor Sonido  │   │       │
          │    │  │   3.3V   ────── VCC Sensor Sonido               │   │       │
          │    │  │   GND    ────── GND (común con fuente 12V)      │   │       │
          │    │  │   VIN    ────── +5V (desde fuente o USB)        │   │       │
          │    │  │                                                  │   │       │
          │    │  └──────────────────────────────────────────────────┘   │       │
          │    └─────────────────────────────────────────────────────────┘       │
          │                                                                      │
          │    ┌─────────────────────────────────────────────────────────┐       │
          │    │              SENSOR DE SONIDO KY-038                    │       │
          │    │  ┌──────────────────────────────────────────────────┐   │       │
          │    │  │    VCC ──────── 3.3V ESP32                       │   │       │
          │    │  │    GND ──────── GND                              │   │       │
          │    │  │    A0  ──────── GPIO34 (Salida Analógica)        │   │       │
          │    │  │    D0  ──────── (No usar - salida digital)       │   │       │
          │    │  └──────────────────────────────────────────────────┘   │       │
          │    └─────────────────────────────────────────────────────────┘       │
          │                                                                      │
          └──────────────────────────────────────────────────────────────────────┘
```

### Conexiones Resumidas

#### ESP32 → MOSFETs (con resistencia 100Ω)

| ESP32 Pin | Resistencia | MOSFET | Color |
|-----------|-------------|--------|-------|
| GPIO25 | 100Ω | Gate Q1 | 🔴 Rojo |
| GPIO26 | 100Ω | Gate Q2 | 🟢 Verde |
| GPIO27 | 100Ω | Gate Q3 | 🔵 Azul |

#### MOSFETs → Tira LED

| MOSFET | Pin Drain | Pin Source |
|--------|-----------|------------|
| Q1 (Rojo) | Cable R de tira | GND común |
| Q2 (Verde) | Cable G de tira | GND común |
| Q3 (Azul) | Cable B de tira | GND común |

#### Sensor de Sonido KY-038

| Sensor Pin | ESP32 Pin |
|------------|-----------|
| VCC | 3.3V |
| GND | GND |
| A0 | GPIO34 |

#### Alimentación

| Componente | Positivo | Negativo |
|------------|----------|----------|
| Tira LED | +12V Fuente | (via MOSFETs) |
| ESP32 | 5V (USB o VIN) | GND común |
| Sensor | 3.3V ESP32 | GND común |

---

## 📱 Configuración de Blynk

### Paso 1: Crear cuenta en Blynk

1. Descarga la app **Blynk IoT** (no la versión legacy)
   - [Android](https://play.google.com/store/apps/details?id=cloud.blynk)
   - [iOS](https://apps.apple.com/app/blynk-iot/id1559317868)

2. Crea una cuenta en [blynk.cloud](https://blynk.cloud)

### Paso 2: Crear Template

1. En Blynk.Cloud → **Templates** → **New Template**
2. Nombre: `RGB LED Controller`
3. Hardware: `ESP32`
4. Connection Type: `WiFi`

### Paso 3: Configurar Datastreams

Crear los siguientes **Datastreams** (Virtual Pins):

| Pin | Nombre | Tipo | Min | Max | Descripción |
|-----|--------|------|-----|-----|-------------|
| V0 | Color | Integer | 0 | 255 | zeRGBa (3 valores) |
| V1 | Brightness | Integer | 0 | 255 | Brillo general |
| V2 | Power | Integer | 0 | 1 | Encendido/Apagado |
| V3 | Mode | Integer | 0 | 5 | Selector de modo |
| V4 | Effect Speed | Integer | 1 | 100 | Velocidad efectos |
| V5 | Mic Sensitivity | Integer | 1 | 100 | Sensibilidad música |
| V6 | Sound Level | Integer | 0 | 4095 | Nivel sonido (lectura) |

### Paso 4: Diseñar Dashboard en la App

Agregar estos **Widgets**:

```
┌─────────────────────────────────────────────────────┐
│                 RGB LED CONTROLLER                  │
├─────────────────────────────────────────────────────┤
│                                                     │
│   ┌─────────────────────────────────────────────┐   │
│   │                                             │   │
│   │              🎨 zeRGBa                      │   │
│   │              (V0 - Merge)                   │   │
│   │                                             │   │
│   └─────────────────────────────────────────────┘   │
│                                                     │
│   ┌─────────────────┐    ┌──────────────────────┐   │
│   │   🔆 Brillo     │    │   ⚡ POWER          │   │
│   │   Slider V1     │    │   Button V2         │   │
│   │   (0-255)       │    │   (Switch)          │   │
│   └─────────────────┘    └──────────────────────┘   │
│                                                     │
│   ┌─────────────────────────────────────────────┐   │
│   │  📋 Modo:  Manual|Música|Rainbow|Fade|...   │   │
│   │  Segmented Switch V3                        │   │
│   └─────────────────────────────────────────────┘   │
│                                                     │
│   ┌─────────────────┐    ┌──────────────────────┐   │
│   │ 🚀 Velocidad    │    │  🎤 Sensibilidad    │   │
│   │   Slider V4     │    │    Slider V5        │   │
│   │   (1-100)       │    │    (1-100)          │   │
│   └─────────────────┘    └──────────────────────┘   │
│                                                     │
│   ┌─────────────────────────────────────────────┐   │
│   │  📊 Nivel de Sonido                         │   │
│   │  Gauge V6 (0-4095)                          │   │
│   └─────────────────────────────────────────────┘   │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Paso 5: Obtener Credenciales

1. **Template** → Copia el `BLYNK_TEMPLATE_ID`
2. **Devices** → **New Device** → **From Template**
3. Copia el `BLYNK_AUTH_TOKEN`

### Paso 6: Actualizar el Código

Edita `src/main.cpp` y reemplaza:

```cpp
#define BLYNK_TEMPLATE_ID "TU_TEMPLATE_ID"      // Ej: "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "RGB LED Controller"
#define BLYNK_AUTH_TOKEN "TU_AUTH_TOKEN"        // Ej: "xxxxxxxx"

const char* ssid = "TU_WIFI_SSID";              // Nombre de tu WiFi
const char* password = "TU_WIFI_PASSWORD";       // Contraseña WiFi
```

---

## 🔧 Instalación y Carga

### Opción 1: PlatformIO (Recomendado)

```bash
# Instalar PlatformIO CLI o usar extensión de VS Code
cd esp32-rgb-led-controller

# Compilar
pio run

# Cargar al ESP32
pio run --target upload

# Monitor serial
pio device monitor
```

### Opción 2: Arduino IDE

1. Instala soporte ESP32: **Archivo** → **Preferencias** → URLs adicionales:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

2. Instala librería Blynk: **Sketch** → **Include Library** → **Manage Libraries** → Busca "Blynk"

3. Selecciona placa: **ESP32 Dev Module**

4. Copia el contenido de `src/main.cpp` a un nuevo sketch

---

## 🎮 Modos de Operación

| Modo | V3 | Descripción |
|------|----|-----------| 
| **Manual** | 0 | Control directo del color con zeRGBa |
| **Música** | 1 | LEDs reaccionan al sonido del ambiente |
| **Rainbow** | 2 | Ciclo continuo de colores arcoíris |
| **Fade** | 3 | Transición suave entre colores predefinidos |
| **Strobe** | 4 | Efecto estroboscópico con el color seleccionado |
| **Fire** | 5 | Simula efecto de fuego/llama |

---

## 🎵 Modo Música - Ajustes

El modo música convierte el sonido en colores:

- **Sonido bajo** → 🔵 Azul → 🟢 Verde
- **Sonido medio** → 🟢 Verde → 🟡 Amarillo  
- **Sonido alto** → 🟠 Naranja → 🔴 Rojo

### Ajustar Sensibilidad

1. Usa el slider **Sensibilidad** (V5) en la app
2. Valores bajos (1-30): Solo reacciona a sonidos fuertes
3. Valores altos (70-100): Reacciona a sonidos suaves

### Calibración del Sensor

Si el sensor KY-038 tiene potenciómetro, ajústalo para que el LED integrado parpadee con la música.

---

## ⚠️ Notas Importantes

### Sobre los MOSFETs

- **IRLZ44N** es ideal porque es "logic-level" (funciona con 3.3V del ESP32)
- Si usas **IRF540N**, necesitas un driver o amplificador ya que requiere >10V en gate
- El **Drain** va a la tira LED, el **Source** a GND

### Sobre la Alimentación

- **NUNCA** alimentes la tira LED desde el ESP32
- Usa una fuente de 12V dedicada
- Conecta los GND de la fuente y del ESP32

### Sobre el Sensor de Sonido

- El **KY-038** tiene salida analógica (A0) y digital (D0)
- Usamos **A0** para mejor respuesta
- GPIO34-39 del ESP32 son solo entrada (perfecto para el sensor)

---

## 🐛 Solución de Problemas

| Problema | Posible Causa | Solución |
|----------|---------------|----------|
| LEDs no encienden | Conexión incorrecta | Verifica polaridad y conexiones |
| Solo un color funciona | MOSFET dañado o mal conectado | Revisa cada canal |
| No conecta a WiFi | Credenciales incorrectas | Verifica SSID y password |
| No conecta a Blynk | Token incorrecto | Verifica BLYNK_AUTH_TOKEN |
| Modo música no responde | Sensor mal conectado | Verifica GPIO34 y alimentación |
| Colores incorrectos | Pines invertidos | Intercambia conexiones R/G/B |

---

## 📝 Licencia

Proyecto de código abierto. Úsalo, modifícalo y compártelo libremente.

---

## 🤝 Créditos

Desarrollado con ayuda de **Kiro** - AI Development Assistant

¡Disfruta tu proyecto! 🎉🌈
