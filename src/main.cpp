/*
 * ESP32 TROLL BOX 😈
 * 3 en 1: Escritor Fantasma + Mouse Jiggler + TV-B-Gone
 * 
 * Funciona como dispositivo USB HID (teclado/mouse)
 * + Transmisor IR para apagar TVs
 * 
 * Control via interfaz web desde tu celular
 */

#include <WiFi.h>
#include <WebServer.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>

// ==================== CONFIGURACIÓN ====================
const char* ssid = "TrollBox_ESP32";      // Nombre del Access Point
const char* password = "12345678";         // Contraseña (mínimo 8 caracteres)

// Pin para LED IR (TV-B-Gone)
#define IR_LED_PIN 4

// ==================== OBJETOS GLOBALES ====================
WebServer server(80);
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;

// Estados
bool ghostWriterEnabled = false;
bool mouseJigglerEnabled = false;
unsigned long lastGhostWrite = 0;
unsigned long lastMouseJiggle = 0;
int ghostWriteInterval = 30000;  // 30 segundos por defecto
int mouseJiggleInterval = 5000;  // 5 segundos por defecto


// Mensajes fantasma para escribir
String ghostMessages[] = {
  "Te estoy observando...",
  "Detras de ti...",
  "Deberias trabajar menos",
  "Lo se todo",
  "No estoy solo",
  "Puedo verte",
  "Hola humano",
  "El WiFi sabe tus secretos",
  "Tu historial es interesante",
  "Necesitas dormir mas",
  "Por que googleaste eso?",
  "Tu jefe esta detras de ti",
  "Mira por la ventana...",
  "Escuchaste eso?",
  "No confies en nadie"
};
int numGhostMessages = 15;

// ==================== CÓDIGOS IR TV-B-Gone ====================
// Códigos de apagado para marcas populares (NEC protocol)
struct TVCode {
  String brand;
  unsigned long code;
  int bits;
};


TVCode tvCodes[] = {
  {"Samsung", 0xE0E040BF, 32},
  {"LG", 0x20DF10EF, 32},
  {"Sony", 0xA90, 12},
  {"Philips", 0x0C, 6},
  {"Panasonic", 0x400401FC, 32},
  {"Toshiba", 0x02FD48B7, 32},
  {"Sharp", 0x41A2, 15},
  {"Sanyo", 0x1C, 6},
  {"Hisense", 0x00FF00FF, 32},
  {"TCL", 0x00FF00FF, 32},
  {"Vizio", 0x20DF10EF, 32},
  {"Roku", 0x57E3E817, 32},
  {"Fire TV", 0x00FF00FF, 32},
  {"Apple TV", 0x77E1, 16}
};
int numTVCodes = 14;

// ==================== FUNCIONES IR ====================
void sendIRPulse(int pulseTime) {
  // Genera pulso de 38kHz
  unsigned long startTime = micros();
  while (micros() - startTime < pulseTime) {
    digitalWrite(IR_LED_PIN, HIGH);
    delayMicroseconds(13);
    digitalWrite(IR_LED_PIN, LOW);
    delayMicroseconds(13);
  }
}


void sendNEC(unsigned long code, int bits) {
  // Header NEC
  sendIRPulse(9000);
  delayMicroseconds(4500);
  
  // Enviar bits
  for (int i = bits - 1; i >= 0; i--) {
    sendIRPulse(560);
    if (code & (1UL << i)) {
      delayMicroseconds(1690);
    } else {
      delayMicroseconds(560);
    }
  }
  
  // Bit de stop
  sendIRPulse(560);
}

void sendAllTVOff() {
  Serial.println("📺 Enviando códigos TV-B-Gone...");
  for (int i = 0; i < numTVCodes; i++) {
    Serial.printf("  Enviando: %s\n", tvCodes[i].brand.c_str());
    sendNEC(tvCodes[i].code, tvCodes[i].bits);
    delay(50);
    // Repetir 3 veces para mejor recepción
    sendNEC(tvCodes[i].code, tvCodes[i].bits);
    delay(50);
    sendNEC(tvCodes[i].code, tvCodes[i].bits);
    delay(100);
  }
  Serial.println("✅ TV-B-Gone completado!");
}


// ==================== FUNCIONES GHOST WRITER ====================
void typeGhostMessage() {
  if (!ghostWriterEnabled) return;
  
  int randomIndex = random(0, numGhostMessages);
  String message = ghostMessages[randomIndex];
  
  Serial.printf("👻 Escribiendo: %s\n", message.c_str());
  
  // Pequeña pausa antes de escribir
  delay(100);
  
  // Escribir caracter por caracter con delays random
  for (int i = 0; i < message.length(); i++) {
    Keyboard.print(message[i]);
    delay(random(50, 150));  // Simula escritura humana
  }
  
  // Enter al final (opcional)
  // Keyboard.press(KEY_RETURN);
  // Keyboard.release(KEY_RETURN);
}

void typeCustomMessage(String message) {
  Serial.printf("⌨️ Escribiendo mensaje personalizado: %s\n", message.c_str());
  for (int i = 0; i < message.length(); i++) {
    Keyboard.print(message[i]);
    delay(random(30, 100));
  }
}


// ==================== FUNCIONES MOUSE JIGGLER ====================
void jiggleMouse() {
  if (!mouseJigglerEnabled) return;
  
  Serial.println("🖱️ Moviendo mouse...");
  
  // Movimiento pequeño aleatorio
  int moveX = random(-5, 6);
  int moveY = random(-5, 6);
  
  Mouse.move(moveX, moveY);
  delay(50);
  Mouse.move(-moveX, -moveY);  // Volver a posición original
}

void crazyMouse() {
  Serial.println("🖱️ MODO LOCO ACTIVADO!");
  for (int i = 0; i < 20; i++) {
    Mouse.move(random(-50, 51), random(-50, 51));
    delay(50);
  }
}

// ==================== ATAJOS DE TECLADO ====================
void openNotepad() {
  // Windows + R
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);
  
  // Escribir notepad
  Keyboard.print("notepad");
  delay(100);
  Keyboard.press(KEY_RETURN);
  Keyboard.release(KEY_RETURN);
}


void openWebsite(String url) {
  // Windows + R
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);
  
  // Escribir URL
  Keyboard.print(url);
  delay(100);
  Keyboard.press(KEY_RETURN);
  Keyboard.release(KEY_RETURN);
}

void rickroll() {
  openWebsite("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
}

void lockScreen() {
  // Windows + L
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('l');
  delay(100);
  Keyboard.releaseAll();
}

void screenshot() {
  // Windows + Shift + S
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press('s');
  delay(100);
  Keyboard.releaseAll();
}


// ==================== PÁGINA WEB ====================
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>😈 ESP32 Troll Box</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
      min-height: 100vh;
      color: white;
      padding: 20px;
    }
    .container { max-width: 500px; margin: 0 auto; }
    h1 { text-align: center; font-size: 2em; margin-bottom: 20px; }
    .card {
      background: rgba(255,255,255,0.1);
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 20px;
      backdrop-filter: blur(10px);
    }
    .card h2 { 
      font-size: 1.3em; 
      margin-bottom: 15px;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .btn {
      width: 100%;
      padding: 15px;
      border: none;
      border-radius: 10px;
      font-size: 1em;
      font-weight: bold;
      cursor: pointer;
      margin-bottom: 10px;
      transition: all 0.3s;
    }
    .btn:active { transform: scale(0.95); }
    .btn-red { background: #e74c3c; color: white; }
    .btn-green { background: #27ae60; color: white; }
    .btn-blue { background: #3498db; color: white; }
    .btn-purple { background: #9b59b6; color: white; }
    .btn-orange { background: #e67e22; color: white; }
    .btn-yellow { background: #f39c12; color: white; }
    .toggle-container {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 10px;
    }
)rawliteral";


const char* htmlPage2 = R"rawliteral(
    .toggle {
      width: 60px;
      height: 30px;
      background: #555;
      border-radius: 15px;
      position: relative;
      cursor: pointer;
      transition: 0.3s;
    }
    .toggle.active { background: #27ae60; }
    .toggle::after {
      content: '';
      position: absolute;
      width: 26px;
      height: 26px;
      background: white;
      border-radius: 50%;
      top: 2px;
      left: 2px;
      transition: 0.3s;
    }
    .toggle.active::after { left: 32px; }
    input[type="text"] {
      width: 100%;
      padding: 12px;
      border: none;
      border-radius: 8px;
      margin-bottom: 10px;
      font-size: 1em;
    }
    .slider-container { margin: 15px 0; }
    .slider-container label { display: block; margin-bottom: 5px; }
    input[type="range"] { width: 100%; }
    .status {
      text-align: center;
      padding: 10px;
      border-radius: 8px;
      margin-top: 10px;
      background: rgba(0,0,0,0.3);
    }
    .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>😈 TROLL BOX</h1>
    
    <!-- TV-B-GONE -->
    <div class="card">
      <h2>📺 TV-B-Gone</h2>
      <button class="btn btn-red" onclick="sendCommand('/tv-off')">
        ⚡ APAGAR TODAS LAS TVs
      </button>
      <p style="font-size:0.8em; opacity:0.7; text-align:center;">
        Envía códigos IR para apagar TVs de todas las marcas
      </p>
    </div>
)rawliteral";


const char* htmlPage3 = R"rawliteral(
    <!-- GHOST WRITER -->
    <div class="card">
      <h2>👻 Escritor Fantasma</h2>
      <div class="toggle-container">
        <span>Auto-escribir mensajes random</span>
        <div id="ghostToggle" class="toggle" onclick="toggleGhost()"></div>
      </div>
      <div class="slider-container">
        <label>Intervalo: <span id="ghostIntervalText">30</span>s</label>
        <input type="range" min="5" max="120" value="30" id="ghostInterval" 
               onchange="updateGhostInterval(this.value)">
      </div>
      <input type="text" id="customMsg" placeholder="Mensaje personalizado...">
      <button class="btn btn-purple" onclick="sendCustomMessage()">
        ⌨️ Escribir Mensaje
      </button>
      <button class="btn btn-orange" onclick="sendCommand('/ghost-now')">
        👻 Mensaje Random AHORA
      </button>
    </div>

    <!-- MOUSE JIGGLER -->
    <div class="card">
      <h2>🖱️ Mouse Jiggler</h2>
      <div class="toggle-container">
        <span>Mover mouse automáticamente</span>
        <div id="mouseToggle" class="toggle" onclick="toggleMouse()"></div>
      </div>
      <div class="slider-container">
        <label>Intervalo: <span id="mouseIntervalText">5</span>s</label>
        <input type="range" min="1" max="30" value="5" id="mouseInterval"
               onchange="updateMouseInterval(this.value)">
      </div>
      <div class="grid">
        <button class="btn btn-blue" onclick="sendCommand('/jiggle')">
          🖱️ Mover Ahora
        </button>
        <button class="btn btn-red" onclick="sendCommand('/crazy-mouse')">
          🤪 MODO LOCO
        </button>
      </div>
    </div>
)rawliteral";


const char* htmlPage4 = R"rawliteral(
    <!-- ATAJOS ESPECIALES -->
    <div class="card">
      <h2>🎯 Atajos Especiales</h2>
      <div class="grid">
        <button class="btn btn-yellow" onclick="sendCommand('/rickroll')">
          🎵 RICKROLL
        </button>
        <button class="btn btn-green" onclick="sendCommand('/notepad')">
          📝 Abrir Notepad
        </button>
        <button class="btn btn-blue" onclick="sendCommand('/lock')">
          🔒 Bloquear PC
        </button>
        <button class="btn btn-purple" onclick="sendCommand('/screenshot')">
          📸 Screenshot
        </button>
      </div>
      <input type="text" id="customUrl" placeholder="URL personalizada..." style="margin-top:10px;">
      <button class="btn btn-orange" onclick="openCustomUrl()">
        🌐 Abrir URL
      </button>
    </div>

    <!-- STATUS -->
    <div class="status" id="status">
      Conectado a TrollBox 😈
    </div>
  </div>

  <script>
    let ghostEnabled = false;
    let mouseEnabled = false;
    
    function sendCommand(endpoint) {
      fetch(endpoint)
        .then(r => r.text())
        .then(t => {
          document.getElementById('status').innerText = t;
        })
        .catch(e => {
          document.getElementById('status').innerText = 'Error: ' + e;
        });
    }
    
    function toggleGhost() {
      ghostEnabled = !ghostEnabled;
      document.getElementById('ghostToggle').classList.toggle('active', ghostEnabled);
      sendCommand('/ghost-toggle?state=' + (ghostEnabled ? '1' : '0'));
    }
    
    function toggleMouse() {
      mouseEnabled = !mouseEnabled;
      document.getElementById('mouseToggle').classList.toggle('active', mouseEnabled);
      sendCommand('/mouse-toggle?state=' + (mouseEnabled ? '1' : '0'));
    }
)rawliteral";


const char* htmlPage5 = R"rawliteral(
    function updateGhostInterval(val) {
      document.getElementById('ghostIntervalText').innerText = val;
      sendCommand('/ghost-interval?val=' + val);
    }
    
    function updateMouseInterval(val) {
      document.getElementById('mouseIntervalText').innerText = val;
      sendCommand('/mouse-interval?val=' + val);
    }
    
    function sendCustomMessage() {
      const msg = document.getElementById('customMsg').value;
      if (msg) {
        sendCommand('/type?msg=' + encodeURIComponent(msg));
        document.getElementById('customMsg').value = '';
      }
    }
    
    function openCustomUrl() {
      const url = document.getElementById('customUrl').value;
      if (url) {
        sendCommand('/open-url?url=' + encodeURIComponent(url));
        document.getElementById('customUrl').value = '';
      }
    }
  </script>
</body>
</html>
)rawliteral";


// ==================== HANDLERS WEB ====================
void handleRoot() {
  String html = String(htmlPage) + String(htmlPage2) + String(htmlPage3) + 
                String(htmlPage4) + String(htmlPage5);
  server.send(200, "text/html", html);
}

void handleTVOff() {
  sendAllTVOff();
  server.send(200, "text/plain", "📺 TV-B-Gone enviado!");
}

void handleGhostToggle() {
  if (server.hasArg("state")) {
    ghostWriterEnabled = server.arg("state") == "1";
  }
  server.send(200, "text/plain", 
    ghostWriterEnabled ? "👻 Ghost Writer ACTIVADO" : "👻 Ghost Writer desactivado");
}

void handleMouseToggle() {
  if (server.hasArg("state")) {
    mouseJigglerEnabled = server.arg("state") == "1";
  }
  server.send(200, "text/plain", 
    mouseJigglerEnabled ? "🖱️ Mouse Jiggler ACTIVADO" : "🖱️ Mouse Jiggler desactivado");
}

void handleGhostInterval() {
  if (server.hasArg("val")) {
    ghostWriteInterval = server.arg("val").toInt() * 1000;
  }
  server.send(200, "text/plain", "⏱️ Intervalo actualizado");
}


void handleMouseInterval() {
  if (server.hasArg("val")) {
    mouseJiggleInterval = server.arg("val").toInt() * 1000;
  }
  server.send(200, "text/plain", "⏱️ Intervalo actualizado");
}

void handleGhostNow() {
  typeGhostMessage();
  server.send(200, "text/plain", "👻 Mensaje enviado!");
}

void handleType() {
  if (server.hasArg("msg")) {
    typeCustomMessage(server.arg("msg"));
  }
  server.send(200, "text/plain", "⌨️ Mensaje escrito!");
}

void handleJiggle() {
  jiggleMouse();
  server.send(200, "text/plain", "🖱️ Mouse movido!");
}

void handleCrazyMouse() {
  crazyMouse();
  server.send(200, "text/plain", "🤪 MODO LOCO ejecutado!");
}

void handleRickroll() {
  rickroll();
  server.send(200, "text/plain", "🎵 RICKROLL enviado!");
}

void handleNotepad() {
  openNotepad();
  server.send(200, "text/plain", "📝 Notepad abierto!");
}


void handleLock() {
  lockScreen();
  server.send(200, "text/plain", "🔒 PC bloqueada!");
}

void handleScreenshot() {
  screenshot();
  server.send(200, "text/plain", "📸 Screenshot tomado!");
}

void handleOpenUrl() {
  if (server.hasArg("url")) {
    openWebsite(server.arg("url"));
  }
  server.send(200, "text/plain", "🌐 URL abierta!");
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  Serial.println("\n");
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.println("║     😈 ESP32 TROLL BOX v1.0 😈       ║");
  Serial.println("║  Ghost Writer + Mouse Jiggler + TV   ║");
  Serial.println("╚═══════════════════════════════════════╝");
  
  // Inicializar USB HID
  USB.begin();
  Keyboard.begin();
  Mouse.begin();
  Serial.println("✅ USB HID inicializado");
  
  // Inicializar IR LED
  pinMode(IR_LED_PIN, OUTPUT);
  digitalWrite(IR_LED_PIN, LOW);
  Serial.println("✅ IR LED configurado en GPIO4");


  // Crear Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("✅ Access Point creado: %s\n", ssid);
  Serial.printf("📱 Conecta tu celular y abre: http://%s\n", IP.toString().c_str());
  
  // Configurar rutas del servidor web
  server.on("/", handleRoot);
  server.on("/tv-off", handleTVOff);
  server.on("/ghost-toggle", handleGhostToggle);
  server.on("/mouse-toggle", handleMouseToggle);
  server.on("/ghost-interval", handleGhostInterval);
  server.on("/mouse-interval", handleMouseInterval);
  server.on("/ghost-now", handleGhostNow);
  server.on("/type", handleType);
  server.on("/jiggle", handleJiggle);
  server.on("/crazy-mouse", handleCrazyMouse);
  server.on("/rickroll", handleRickroll);
  server.on("/notepad", handleNotepad);
  server.on("/lock", handleLock);
  server.on("/screenshot", handleScreenshot);
  server.on("/open-url", handleOpenUrl);
  
  server.begin();
  Serial.println("✅ Servidor web iniciado");
  Serial.println("\n🎮 ¡TROLL BOX LISTO!");
  Serial.println("════════════════════════════════════════\n");
}


// ==================== LOOP ====================
void loop() {
  server.handleClient();
  
  unsigned long currentMillis = millis();
  
  // Ghost Writer automático
  if (ghostWriterEnabled && (currentMillis - lastGhostWrite >= ghostWriteInterval)) {
    typeGhostMessage();
    lastGhostWrite = currentMillis;
  }
  
  // Mouse Jiggler automático
  if (mouseJigglerEnabled && (currentMillis - lastMouseJiggle >= mouseJiggleInterval)) {
    jiggleMouse();
    lastMouseJiggle = currentMillis;
  }
  
  delay(10);
}
