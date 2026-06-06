/*
 * TV-B-GONE Simple para ESP32-WROOM (tu placa actual)
 * 
 * ✅ Funciona con tu ESP32 HW-394
 * ✅ Solo necesitas 1 LED infrarrojo + resistencia 100Ω
 * 
 * Conexión:
 * GPIO4 ──── 100Ω ──── LED IR (pata larga +) ──── GND
 */

#include <WiFi.h>
#include <WebServer.h>

// Configuración
const char* ssid = "TV_Killer";
const char* password = "12345678";
#define IR_LED_PIN 4

WebServer server(80);

// ==================== CÓDIGOS IR ====================
struct TVCode {
  const char* brand;
  unsigned long code;
  int bits;
};

TVCode tvCodes[] = {
  {"Samsung", 0xE0E040BF, 32},
  {"LG", 0x20DF10EF, 32},
  {"Sony", 0xA90, 12},
  {"Philips RC5", 0x0C, 6},
  {"Philips RC6", 0x100C, 16},
  {"Panasonic", 0x400401FC, 32},
  {"Toshiba", 0x02FD48B7, 32},
  {"Sharp", 0x41A2, 15},
  {"Sanyo", 0x1C, 6},
  {"Hisense", 0xFD02FB04, 32},
  {"TCL", 0xF708FB04, 32},
  {"Vizio", 0x20DF10EF, 32},
  {"Haier", 0x19E6E817, 32},
  {"JVC", 0xC5E8, 16},
  {"Hitachi", 0x0AF5, 16},
  {"Magnavox", 0x20DF10EF, 32},
  {"Emerson", 0xF708FB04, 32},
  {"RCA", 0x0FF00FF0, 32},
  {"Zenith", 0x20DF10EF, 32},
  {"Insignia", 0xE0E040BF, 32},
  {"Westinghouse", 0x20DF10EF, 32},
  {"Element", 0x02FDFE01, 32},
  {"Sceptre", 0x00FF00FF, 32},
  {"Roku TV", 0x57E3E817, 32},
  {"Fire TV", 0xF0700F08, 32},
  {"Apple TV", 0x77E150AF, 32},
  {"Chromecast", 0x20DF10EF, 32}
};
const int numCodes = sizeof(tvCodes) / sizeof(tvCodes[0]);


int tvOffCount = 0;  // Contador de veces usado

// ==================== FUNCIONES IR ====================
void sendIRPulse(unsigned int microsecs) {
  // Genera señal de 38kHz
  unsigned long startTime = micros();
  while (micros() - startTime < microsecs) {
    digitalWrite(IR_LED_PIN, HIGH);
    delayMicroseconds(13);  // 38kHz = 26us periodo, 50% duty = 13us
    digitalWrite(IR_LED_PIN, LOW);
    delayMicroseconds(13);
  }
}

void sendNEC(unsigned long code, int bits) {
  // Protocolo NEC
  sendIRPulse(9000);      // Header mark
  delayMicroseconds(4500); // Header space
  
  for (int i = bits - 1; i >= 0; i--) {
    sendIRPulse(560);      // Bit mark
    if (code & (1UL << i)) {
      delayMicroseconds(1690);  // 1 = space largo
    } else {
      delayMicroseconds(560);   // 0 = space corto
    }
  }
  sendIRPulse(560);  // Stop bit
}

void sendSony(unsigned long code, int bits) {
  // Protocolo Sony SIRC
  sendIRPulse(2400);  // Header
  delayMicroseconds(600);
  
  for (int i = bits - 1; i >= 0; i--) {
    if (code & (1UL << i)) {
      sendIRPulse(1200);  // 1
    } else {
      sendIRPulse(600);   // 0
    }
    delayMicroseconds(600);
  }
}


void sendRC5(unsigned long code, int bits) {
  // Protocolo RC5 (Philips)
  for (int i = bits - 1; i >= 0; i--) {
    if (code & (1UL << i)) {
      delayMicroseconds(889);
      sendIRPulse(889);
    } else {
      sendIRPulse(889);
      delayMicroseconds(889);
    }
  }
}

void sendAllTVOff() {
  Serial.println("\n📺 ══════════════════════════════════");
  Serial.println("📺 INICIANDO TV-B-GONE...");
  Serial.println("📺 ══════════════════════════════════\n");
  
  for (int i = 0; i < numCodes; i++) {
    Serial.printf("  📡 Enviando: %s\n", tvCodes[i].brand);
    
    // Enviar 3 veces cada código para mejor recepción
    for (int repeat = 0; repeat < 3; repeat++) {
      if (strcmp(tvCodes[i].brand, "Sony") == 0) {
        sendSony(tvCodes[i].code, tvCodes[i].bits);
      } else if (strstr(tvCodes[i].brand, "RC5") != NULL) {
        sendRC5(tvCodes[i].code, tvCodes[i].bits);
      } else {
        sendNEC(tvCodes[i].code, tvCodes[i].bits);
      }
      delay(40);
    }
    delay(50);
  }
  
  tvOffCount++;
  Serial.println("\n📺 ══════════════════════════════════");
  Serial.printf("📺 ¡COMPLETADO! (usado %d veces)\n", tvOffCount);
  Serial.println("📺 ══════════════════════════════════\n");
}


// ==================== PÁGINA WEB ====================
String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>📺 TV-B-Gone</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(135deg, #0f0f23 0%, #1a1a3e 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 20px;
    }
    .container {
      text-align: center;
      color: white;
    }
    h1 {
      font-size: 2.5em;
      margin-bottom: 10px;
      text-shadow: 0 0 20px #ff0000;
    }
    .subtitle {
      opacity: 0.7;
      margin-bottom: 40px;
    }
    .big-button {
      width: 250px;
      height: 250px;
      border-radius: 50%;
      border: none;
      background: linear-gradient(145deg, #ff4444, #cc0000);
      color: white;
      font-size: 1.5em;
      font-weight: bold;
      cursor: pointer;
      box-shadow: 0 10px 40px rgba(255, 0, 0, 0.4),
                  inset 0 -5px 20px rgba(0, 0, 0, 0.3);
      transition: all 0.2s;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
    }
    .big-button:hover {
      transform: scale(1.05);
      box-shadow: 0 15px 50px rgba(255, 0, 0, 0.6);
    }
    .big-button:active {
      transform: scale(0.95);
    }
    .big-button .icon { font-size: 4em; margin-bottom: 10px; }
    .status {
      margin-top: 30px;
      padding: 15px 30px;
      background: rgba(255,255,255,0.1);
      border-radius: 10px;
      min-height: 50px;
    }
    .brands {
      margin-top: 30px;
      font-size: 0.8em;
      opacity: 0.5;
      max-width: 300px;
    }
    .counter { font-size: 1.2em; margin-top: 20px; opacity: 0.8; }
    @keyframes pulse {
      0% { box-shadow: 0 0 0 0 rgba(255, 0, 0, 0.7); }
      70% { box-shadow: 0 0 0 30px rgba(255, 0, 0, 0); }
      100% { box-shadow: 0 0 0 0 rgba(255, 0, 0, 0); }
    }
    .sending { animation: pulse 0.5s infinite; }
  </style>
</head>
<body>
  <div class="container">
    <h1>📺 TV-B-GONE</h1>
    <p class="subtitle">Apaga cualquier TV a tu alrededor</p>
    
    <button class="big-button" id="btn" onclick="sendOff()">
      <span class="icon">⚡</span>
      <span>APAGAR TVs</span>
    </button>
    
    <div class="status" id="status">Listo para usar</div>
    <div class="counter">Usado: <span id="count">)rawliteral";
  
  html += String(tvOffCount);
  
  html += R"rawliteral(</span> veces</div>
    
    <p class="brands">
      Samsung • LG • Sony • Philips • Panasonic • Toshiba • Sharp • 
      Hisense • TCL • Vizio • Roku • Fire TV • Apple TV y más...
    </p>
  </div>
  
  <script>
    function sendOff() {
      const btn = document.getElementById('btn');
      const status = document.getElementById('status');
      
      btn.classList.add('sending');
      status.innerText = '📡 Enviando códigos IR...';
      btn.disabled = true;
      
      fetch('/off')
        .then(r => r.text())
        .then(t => {
          status.innerText = '✅ ¡Códigos enviados! Las TVs deberían apagarse';
          document.getElementById('count').innerText = 
            parseInt(document.getElementById('count').innerText) + 1;
        })
        .catch(e => {
          status.innerText = '❌ Error: ' + e;
        })
        .finally(() => {
          btn.classList.remove('sending');
          btn.disabled = false;
          setTimeout(() => {
            status.innerText = 'Listo para usar';
          }, 3000);
        });
    }
  </script>
</body>
</html>
)rawliteral";
  
  return html;
}


// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n");
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.println("║      📺 TV-B-GONE para ESP32 📺       ║");
  Serial.println("║    Apaga cualquier TV a tu alrededor  ║");
  Serial.println("╚═══════════════════════════════════════╝\n");
  
  // Configurar LED IR
  pinMode(IR_LED_PIN, OUTPUT);
  digitalWrite(IR_LED_PIN, LOW);
  Serial.println("✅ LED IR configurado en GPIO4");
  
  // Crear Access Point
  WiFi.softAP(ssid, password);
  Serial.printf("✅ WiFi AP creado: %s (pass: %s)\n", ssid, password);
  Serial.printf("📱 Conecta y abre: http://%s\n", WiFi.softAPIP().toString().c_str());
  
  // Rutas web
  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });
  
  server.on("/off", []() {
    sendAllTVOff();
    server.send(200, "text/plain", "OK");
  });
  
  server.begin();
  
  Serial.println("\n🎮 ¡TV-B-GONE LISTO!");
  Serial.println("   Conecta tu celular al WiFi y presiona el botón");
  Serial.println("═══════════════════════════════════════════════════\n");
}

void loop() {
  server.handleClient();
}
