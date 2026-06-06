/*
 * ESP32 RGB LED Controller
 * Control por Blynk + Modo Reactivo a Música
 * 
 * Autor: Kiro Assistant
 * Hardware: ESP32 + Tira LED RGB 2835 12V + Sensor de Sonido
 */

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TU_TEMPLATE_ID"      // Obtener de Blynk.Cloud
#define BLYNK_TEMPLATE_NAME "RGB LED Controller"
#define BLYNK_AUTH_TOKEN "TU_AUTH_TOKEN"        // Obtener de Blynk.Cloud

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ==================== CONFIGURACIÓN WiFi ====================
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";

// ==================== PINES GPIO ====================
// Pines PWM para MOSFETs (control de colores)
#define PIN_RED    25
#define PIN_GREEN  26
#define PIN_BLUE   27

// Pin para sensor de sonido (micrófono analógico)
#define PIN_MIC    34  // ADC1_CH6 - Solo lectura analógica

// ==================== CONFIGURACIÓN PWM ====================
// ESP32 tiene 16 canales PWM disponibles
#define PWM_FREQ       5000   // Frecuencia PWM en Hz
#define PWM_RESOLUTION 8      // Resolución 8 bits (0-255)
#define PWM_CH_RED     0
#define PWM_CH_GREEN   1
#define PWM_CH_BLUE    2

// ==================== VARIABLES GLOBALES ====================
// Colores actuales (0-255)
uint8_t redValue = 0;
uint8_t greenValue = 0;
uint8_t blueValue = 0;
uint8_t brightness = 255;  // Brillo general

// Modos de operación
enum Mode {
  MODE_MANUAL,      // Control manual por Blynk
  MODE_MUSIC,       // Reactivo a música
  MODE_RAINBOW,     // Efecto arcoíris
  MODE_FADE,        // Fade entre colores
  MODE_STROBE,      // Efecto estroboscópico
  MODE_FIRE         // Efecto fuego
};

Mode currentMode = MODE_MANUAL;
bool ledEnabled = true;

// Variables para modo música
#define MIC_SAMPLES 64
int micSamples[MIC_SAMPLES];
int micIndex = 0;
int micThreshold = 1500;      // Umbral de sensibilidad
int micSensitivity = 50;      // Sensibilidad (1-100)
float smoothedLevel = 0;
unsigned long lastMusicUpdate = 0;

// Variables para efectos
unsigned long lastEffectUpdate = 0;
int effectSpeed = 50;         // Velocidad de efectos (1-100)
float hue = 0;                // Para efecto rainbow
int fadeStep = 0;             // Para efecto fade

// Colores predefinidos para efectos
const uint8_t fadeColors[][3] = {
  {255, 0, 0},      // Rojo
  {255, 127, 0},    // Naranja
  {255, 255, 0},    // Amarillo
  {0, 255, 0},      // Verde
  {0, 255, 255},    // Cian
  {0, 0, 255},      // Azul
  {127, 0, 255},    // Violeta
  {255, 0, 255}     // Magenta
};
#define NUM_FADE_COLORS (sizeof(fadeColors) / sizeof(fadeColors[0]))

// Timer para Blynk
BlynkTimer timer;

// ==================== FUNCIONES DE COLOR ====================

// Aplicar color a los LEDs
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  if (!ledEnabled) {
    ledcWrite(PWM_CH_RED, 0);
    ledcWrite(PWM_CH_GREEN, 0);
    ledcWrite(PWM_CH_BLUE, 0);
    return;
  }
  
  // Aplicar brillo
  uint8_t adjR = (r * brightness) / 255;
  uint8_t adjG = (g * brightness) / 255;
  uint8_t adjB = (b * brightness) / 255;
  
  ledcWrite(PWM_CH_RED, adjR);
  ledcWrite(PWM_CH_GREEN, adjG);
  ledcWrite(PWM_CH_BLUE, adjB);
}

// Convertir HSV a RGB
void hsvToRgb(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  
  float rf, gf, bf;
  
  if (h < 60) {
    rf = c; gf = x; bf = 0;
  } else if (h < 120) {
    rf = x; gf = c; bf = 0;
  } else if (h < 180) {
    rf = 0; gf = c; bf = x;
  } else if (h < 240) {
    rf = 0; gf = x; bf = c;
  } else if (h < 300) {
    rf = x; gf = 0; bf = c;
  } else {
    rf = c; gf = 0; bf = x;
  }
  
  r = (rf + m) * 255;
  g = (gf + m) * 255;
  b = (bf + m) * 255;
}

// ==================== MODO MÚSICA ====================

// Leer nivel de sonido del micrófono
int readMicLevel() {
  int minVal = 4095;
  int maxVal = 0;
  
  // Tomar varias muestras para obtener amplitud pico a pico
  for (int i = 0; i < 32; i++) {
    int sample = analogRead(PIN_MIC);
    if (sample < minVal) minVal = sample;
    if (sample > maxVal) maxVal = sample;
    delayMicroseconds(100);
  }
  
  return maxVal - minVal;  // Amplitud pico a pico
}

// Procesar modo música
void processMusicMode() {
  int level = readMicLevel();
  
  // Suavizado exponencial
  smoothedLevel = smoothedLevel * 0.7 + level * 0.3;
  
  // Mapear nivel a color
  // Ajustar según sensibilidad
  int adjustedLevel = map(micSensitivity, 1, 100, level / 4, level);
  adjustedLevel = constrain(adjustedLevel, 0, 4095);
  
  // Mapear a intensidad (0-255)
  int intensity = map(adjustedLevel, 0, micThreshold, 0, 255);
  intensity = constrain(intensity, 0, 255);
  
  // Colores según intensidad del sonido
  // Bajo: azul/verde, Medio: amarillo/naranja, Alto: rojo
  uint8_t r, g, b;
  
  if (intensity < 85) {
    // Bajo - Azul a Verde
    r = 0;
    g = map(intensity, 0, 85, 0, 255);
    b = map(intensity, 0, 85, 255, 0);
  } else if (intensity < 170) {
    // Medio - Verde a Amarillo
    r = map(intensity, 85, 170, 0, 255);
    g = 255;
    b = 0;
  } else {
    // Alto - Amarillo a Rojo
    r = 255;
    g = map(intensity, 170, 255, 255, 0);
    b = 0;
  }
  
  setColor(r, g, b);
}

// ==================== EFECTOS ====================

// Efecto Arcoíris
void processRainbow() {
  uint8_t r, g, b;
  hsvToRgb(hue, 1.0, 1.0, r, g, b);
  setColor(r, g, b);
  
  hue += map(effectSpeed, 1, 100, 0.5, 5);
  if (hue >= 360) hue = 0;
}

// Efecto Fade entre colores
void processFade() {
  static float transition = 0;
  static int currentColor = 0;
  
  int nextColor = (currentColor + 1) % NUM_FADE_COLORS;
  
  // Interpolar entre color actual y siguiente
  uint8_t r = fadeColors[currentColor][0] + (fadeColors[nextColor][0] - fadeColors[currentColor][0]) * transition;
  uint8_t g = fadeColors[currentColor][1] + (fadeColors[nextColor][1] - fadeColors[currentColor][1]) * transition;
  uint8_t b = fadeColors[currentColor][2] + (fadeColors[nextColor][2] - fadeColors[currentColor][2]) * transition;
  
  setColor(r, g, b);
  
  transition += map(effectSpeed, 1, 100, 0.005, 0.05);
  if (transition >= 1.0) {
    transition = 0;
    currentColor = nextColor;
  }
}

// Efecto Estroboscópico
void processStrobe() {
  static bool strobeOn = false;
  strobeOn = !strobeOn;
  
  if (strobeOn) {
    setColor(redValue, greenValue, blueValue);
  } else {
    setColor(0, 0, 0);
  }
}

// Efecto Fuego
void processFire() {
  // Generar parpadeo aleatorio tipo fuego
  int flicker = random(0, 150);
  int r = 255 - flicker;
  int g = 80 - flicker / 2;
  int b = 0;
  
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  
  setColor(r, g, b);
}

// ==================== BLYNK CALLBACKS ====================

// V0: Widget ZeRGBa o Color Picker (Merge mode, valores 0-255)
BLYNK_WRITE(V0) {
  redValue = param[0].asInt();
  greenValue = param[1].asInt();
  blueValue = param[2].asInt();
  
  if (currentMode == MODE_MANUAL) {
    setColor(redValue, greenValue, blueValue);
  }
  
  Serial.printf("Color: R=%d, G=%d, B=%d\n", redValue, greenValue, blueValue);
}

// V1: Slider de Brillo (0-255)
BLYNK_WRITE(V1) {
  brightness = param.asInt();
  
  if (currentMode == MODE_MANUAL) {
    setColor(redValue, greenValue, blueValue);
  }
  
  Serial.printf("Brillo: %d\n", brightness);
}

// V2: Botón ON/OFF
BLYNK_WRITE(V2) {
  ledEnabled = param.asInt();
  
  if (!ledEnabled) {
    setColor(0, 0, 0);
  } else if (currentMode == MODE_MANUAL) {
    setColor(redValue, greenValue, blueValue);
  }
  
  Serial.printf("LED: %s\n", ledEnabled ? "ON" : "OFF");
}

// V3: Selector de Modo (Menu o Segmented Switch)
// 0=Manual, 1=Música, 2=Rainbow, 3=Fade, 4=Strobe, 5=Fire
BLYNK_WRITE(V3) {
  int mode = param.asInt();
  currentMode = (Mode)mode;
  
  const char* modeNames[] = {"Manual", "Música", "Rainbow", "Fade", "Strobe", "Fire"};
  Serial.printf("Modo: %s\n", modeNames[mode]);
}

// V4: Slider de Velocidad de Efectos (1-100)
BLYNK_WRITE(V4) {
  effectSpeed = param.asInt();
  Serial.printf("Velocidad: %d\n", effectSpeed);
}

// V5: Slider de Sensibilidad del Micrófono (1-100)
BLYNK_WRITE(V5) {
  micSensitivity = param.asInt();
  micThreshold = map(micSensitivity, 1, 100, 3000, 500);
  Serial.printf("Sensibilidad Mic: %d (threshold: %d)\n", micSensitivity, micThreshold);
}

// V6: Display del nivel de sonido (solo lectura)
// Se actualiza automáticamente

// Callback cuando se conecta a Blynk
BLYNK_CONNECTED() {
  // Sincronizar estados desde la app
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5);
  Serial.println("Conectado a Blynk - Estados sincronizados");
}

// ==================== FUNCIONES PRINCIPALES ====================

// Función llamada por timer para actualizar efectos
void updateEffects() {
  unsigned long currentMillis = millis();
  int interval;
  
  switch (currentMode) {
    case MODE_MUSIC:
      processMusicMode();
      // Enviar nivel a Blynk para visualización
      Blynk.virtualWrite(V6, (int)smoothedLevel);
      break;
      
    case MODE_RAINBOW:
      interval = map(effectSpeed, 1, 100, 100, 10);
      if (currentMillis - lastEffectUpdate >= interval) {
        processRainbow();
        lastEffectUpdate = currentMillis;
      }
      break;
      
    case MODE_FADE:
      interval = map(effectSpeed, 1, 100, 50, 5);
      if (currentMillis - lastEffectUpdate >= interval) {
        processFade();
        lastEffectUpdate = currentMillis;
      }
      break;
      
    case MODE_STROBE:
      interval = map(effectSpeed, 1, 100, 200, 20);
      if (currentMillis - lastEffectUpdate >= interval) {
        processStrobe();
        lastEffectUpdate = currentMillis;
      }
      break;
      
    case MODE_FIRE:
      interval = map(effectSpeed, 1, 100, 150, 30);
      if (currentMillis - lastEffectUpdate >= interval) {
        processFire();
        lastEffectUpdate = currentMillis;
      }
      break;
      
    case MODE_MANUAL:
    default:
      // En modo manual, el color se controla directamente por Blynk
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n========================================");
  Serial.println("ESP32 RGB LED Controller");
  Serial.println("Blynk + Modo Música");
  Serial.println("========================================\n");
  
  // Configurar pines PWM para LEDs
  ledcSetup(PWM_CH_RED, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CH_GREEN, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CH_BLUE, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(PIN_RED, PWM_CH_RED);
  ledcAttachPin(PIN_GREEN, PWM_CH_GREEN);
  ledcAttachPin(PIN_BLUE, PWM_CH_BLUE);
  
  // Configurar pin del micrófono
  analogReadResolution(12);  // 12 bits (0-4095)
  analogSetAttenuation(ADC_11db);  // Rango completo 0-3.3V
  pinMode(PIN_MIC, INPUT);
  
  // Test inicial - parpadeo RGB
  Serial.println("Test de LEDs...");
  setColor(255, 0, 0); delay(300);
  setColor(0, 255, 0); delay(300);
  setColor(0, 0, 255); delay(300);
  setColor(0, 0, 0);
  Serial.println("Test completado!");
  
  // Conectar a WiFi
  Serial.printf("Conectando a WiFi: %s", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" ¡Conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" Error de conexión WiFi");
  }
  
  // Conectar a Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
  
  // Configurar timer para actualizar efectos cada 10ms
  timer.setInterval(10L, updateEffects);
  
  // Color inicial
  redValue = 255;
  greenValue = 100;
  blueValue = 0;
  setColor(redValue, greenValue, blueValue);
  
  Serial.println("\n¡Sistema listo!");
  Serial.println("Controla desde la app Blynk");
}

void loop() {
  Blynk.run();
  timer.run();
}
