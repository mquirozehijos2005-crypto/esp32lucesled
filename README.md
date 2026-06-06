# 😈 ESP32 TROLL BOX

**3 herramientas de bromas en 1:**
- 📺 **TV-B-Gone** - Apaga cualquier TV a tu alrededor
- 👻 **Escritor Fantasma** - Escribe mensajes espeluznantes en PCs
- 🖱️ **Mouse Jiggler** - Mueve el mouse remotamente

Control total desde tu celular via WiFi 📱

---

## ⚠️ IMPORTANTE: Compatibilidad de Hardware

| Función | ESP32-WROOM (HW-394) | ESP32-S2 | ESP32-S3 |
|---------|---------------------|----------|----------|
| 📺 TV-B-Gone | ✅ Sí | ✅ Sí | ✅ Sí |
| 👻 Ghost Writer | ❌ No* | ✅ Sí | ✅ Sí |
| 🖱️ Mouse Jiggler | ❌ No* | ✅ Sí | ✅ Sí |

**\*El ESP32 clásico NO tiene USB HID nativo.** Para Ghost Writer y Mouse Jiggler necesitas:
- **Opción 1:** Comprar un ESP32-S2 o ESP32-S3 (~$5.000-8.000 CLP)
- **Opción 2:** Agregar módulo CH9329 (~$3.000 CLP) - Convierte UART a USB HID

### 🎯 Con tu ESP32-WROOM actual puedes usar:
- ✅ TV-B-Gone (solo necesitas 1 LED infrarrojo)
- ✅ Interfaz web de control
- ✅ WiFi trolling (crear redes falsas, etc.)

---

## 📺 TV-B-GONE (Funciona en CUALQUIER ESP32)


### Componentes necesarios:
| Componente | Precio | Dónde comprar |
|------------|--------|---------------|
| LED Infrarrojo 5mm | $200 | Cualquier electrónica |
| Resistencia 100Ω | $50 | Cualquier electrónica |

### Conexión:
```
ESP32 GPIO4 ──── 100Ω ──── LED IR (+) ──── GND
                            (pata larga)
```

### Marcas soportadas:
Samsung, LG, Sony, Philips, Panasonic, Toshiba, Sharp, Sanyo, Hisense, TCL, Vizio, Roku TV, Fire TV, Apple TV

---

## 🚀 Instalación Rápida

### Para TV-B-Gone solamente (tu ESP32 actual):


Crea un archivo simplificado `tv_b_gone_simple.cpp`:

```cpp
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TV-B-Gone";
const char* password = "12345678";
#define IR_LED_PIN 4

WebServer server(80);

// Códigos IR de TVs
struct TVCode { String brand; unsigned long code; int bits; };
TVCode tvCodes[] = {
  {"Samsung", 0xE0E040BF, 32}, {"LG", 0x20DF10EF, 32},
  {"Sony", 0xA90, 12}, {"Panasonic", 0x400401FC, 32},
  {"Philips", 0x0C, 6}, {"Toshiba", 0x02FD48B7, 32}
};

void sendIRPulse(int t) {
  unsigned long start = micros();
  while (micros() - start < t) {
    digitalWrite(IR_LED_PIN, HIGH); delayMicroseconds(13);
    digitalWrite(IR_LED_PIN, LOW); delayMicroseconds(13);
  }
}

void sendNEC(unsigned long code, int bits) {
  sendIRPulse(9000); delayMicroseconds(4500);
  for (int i = bits - 1; i >= 0; i--) {
    sendIRPulse(560);
    delayMicroseconds((code & (1UL << i)) ? 1690 : 560);
  }
  sendIRPulse(560);
}

void sendAllTVOff() {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      sendNEC(tvCodes[i].code, tvCodes[i].bits);
      delay(40);
    }
  }
}

void setup() {
  pinMode(IR_LED_PIN, OUTPUT);
  WiFi.softAP(ssid, password);
  server.on("/", []() {
    server.send(200, "text/html", 
      "<h1>TV-B-Gone</h1><button onclick=\"fetch('/off')\">APAGAR TVs</button>");
  });
  server.on("/off", []() { sendAllTVOff(); server.send(200, "text/plain", "OK"); });
  server.begin();
}

void loop() { server.handleClient(); }
```

---

## 📱 Cómo Usar

1. **Sube el código** al ESP32
2. **Conecta tu celular** a la red WiFi `TrollBox_ESP32` (contraseña: `12345678`)
3. **Abre el navegador** y ve a `http://192.168.4.1`
4. **¡Trollea!** 😈

---

## 🎮 Funciones Disponibles

### 📺 TV-B-Gone
- **APAGAR TODAS LAS TVs**: Envía códigos IR a todas las marcas conocidas
- Rango: ~5-10 metros (aumentable con más LEDs IR)

### 👻 Escritor Fantasma (requiere ESP32-S2/S3)
- **Auto-escribir**: Escribe mensajes random cada X segundos
- **Mensaje personalizado**: Escribe lo que quieras
- **Mensaje random ahora**: Escribe un mensaje espeluznante inmediatamente

Mensajes incluidos:
- "Te estoy observando..."
- "Detrás de ti..."
- "Por qué googleaste eso?"
- "Tu jefe está detrás de ti"
- Y muchos más...

### 🖱️ Mouse Jiggler (requiere ESP32-S2/S3)
- **Auto-mover**: Mueve el mouse cada X segundos (evita suspensión)
- **Mover ahora**: Un movimiento pequeño
- **MODO LOCO**: Mueve el mouse como poseído 🤪

### 🎯 Atajos Especiales (requiere ESP32-S2/S3)
- **RICKROLL**: Abre YouTube con "Never Gonna Give You Up"
- **Abrir Notepad**: Abre bloc de notas en Windows
- **Bloquear PC**: Win+L
- **Screenshot**: Win+Shift+S
- **URL personalizada**: Abre cualquier página web


---

## 🔧 Alternativa: Módulo CH9329 para ESP32 clásico

Si no quieres comprar un ESP32-S2, puedes agregar un **CH9329**:

```
ESP32 TX (GPIO17) ──── RX CH9329
ESP32 RX (GPIO16) ──── TX CH9329
ESP32 3.3V ─────────── VCC CH9329
ESP32 GND ──────────── GND CH9329
CH9329 USB ─────────── PC víctima
```

El CH9329 convierte comandos seriales en USB HID (teclado/mouse).

---

## ⚠️ Disclaimer

Este proyecto es **SOLO PARA FINES EDUCATIVOS Y DE ENTRETENIMIENTO**.

- ✅ Úsalo con tus propios dispositivos
- ✅ Úsalo para bromas inofensivas entre amigos
- ❌ NO lo uses para actividades maliciosas
- ❌ NO lo uses en equipos ajenos sin permiso
- ❌ NO lo uses en lugares públicos de forma disruptiva

**El autor no se hace responsable del mal uso de este código.**

---

## 📝 Licencia

Proyecto open source. Úsalo, modifícalo, compártelo.

---

## 🤝 Créditos

Desarrollado con ayuda de **Kiro** - AI Development Assistant

¡Que disfrutes trolleando (responsablemente)! 😈🎉
