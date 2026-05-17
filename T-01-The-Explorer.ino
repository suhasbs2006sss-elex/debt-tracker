/*
 * ================================================================
 *   SENTINEL — Final AI Incident Detection System
 *   Board   : ESP32-CAM AI-Thinker + MB Module
 *
 *   Features:
 *   ✅ 6 Sensor fusion (Flame/Sound/PIR/Shock/DHT11/LDR)
 *   ✅ Edge Impulse AI camera classification
 *   ✅ ISD1820 Voice alert
 *   ✅ Telegram Bot alert with full sensor + AI report
 *
 *   PIN CONNECTIONS
 *   ─────────────────────────────────────────
 *   Flame   (KY-026) DO    → GPIO 13
 *   Sound   (KY-037) DO    → GPIO 14
 *   PIR   (HC-SR501) OUT   → GPIO 12  (VCC = VIN)
 *   Shock   (KY-002) DO    → GPIO 15
 *   DHT11   (KY-015) DATA  → GPIO 2
 *   LDR     (KY-018) DO    → GPIO 4
 *   ISD1820          PLAY-E → GPIO 16  (VCC = VIN)
 *   ─────────────────────────────────────────
 *   Libraries: disaster_detection_inferencing
 *              DHT sensor library by Adafruit
 * ================================================================
 */

/* ── Edge Impulse Includes ─────────────────────────────────── */
#include <disaster_detection_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"

/* ── Other Includes ─────────────────────────────────────────── */
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

/* ── WiFi & Telegram Credentials ───────────────────────────── */
const char*   WIFI_SSID  = "Nothing Phone 3a";
const char*   WIFI_PASS  = "suhassuhas";
const String  BOT_TOKEN  = "8360689377:AAHvBPzKNnGHLfN-t-je-f3BNGy3mkKkm6M";
const String  CHAT_ID    = "6416869653";

/* ── Camera Pin Definitions (AI Thinker) ───────────────────── */
#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

/* ── Sensor Pin Definitions ────────────────────────────────── */
#define FLAME_PIN     13
#define SOUND_PIN     14
#define PIR_PIN       12
#define SHOCK_PIN     15
#define DHT_PIN        2
#define LDR_PIN        4
#define ISD1820_PIN   16

/* ── DHT11 ─────────────────────────────────────────────────── */
DHT dht(DHT_PIN, DHT11);

/* ── Thresholds ─────────────────────────────────────────────── */
#define TEMP_DANGER      38.0
#define TEMP_SUSPICIOUS  33.0
#define AI_CONFIDENCE    0.6     // Minimum confidence to trust AI result

/* ── Cooldown Timers ────────────────────────────────────────── */
#define ISD_COOLDOWN       15000
#define TELEGRAM_COOLDOWN  60000
unsigned long lastISDTime      = 0;
unsigned long lastTelegramTime = 0;

/* ── Camera Globals ─────────────────────────────────────────── */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS  320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS  240
#define EI_CAMERA_FRAME_BYTE_SIZE        3

static bool    debug_nn      = false;
static bool    is_initialised = false;
uint8_t       *snapshot_buf;

static camera_config_t camera_config = {
    .pin_pwdn     = PWDN_GPIO_NUM,
    .pin_reset    = RESET_GPIO_NUM,
    .pin_xclk     = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7       = Y9_GPIO_NUM,
    .pin_d6       = Y8_GPIO_NUM,
    .pin_d5       = Y7_GPIO_NUM,
    .pin_d4       = Y6_GPIO_NUM,
    .pin_d3       = Y5_GPIO_NUM,
    .pin_d2       = Y4_GPIO_NUM,
    .pin_d1       = Y3_GPIO_NUM,
    .pin_d0       = Y2_GPIO_NUM,
    .pin_vsync    = VSYNC_GPIO_NUM,
    .pin_href     = HREF_GPIO_NUM,
    .pin_pclk     = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size   = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count     = 1,
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
};

/* ── Situation Enum ─────────────────────────────────────────── */
enum Situation { NORMAL, SUSPICIOUS, DANGEROUS };

/* ── Camera Function Declarations ───────────────────────────── */
bool   ei_camera_init(void);
void   ei_camera_deinit(void);
bool   ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

/* ================================================================
    WIFI CONNECT
   ================================================================ */
void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500); Serial.print("."); attempts++;
  }
  if (WiFi.status() == WL_CONNECTED)
    Serial.println(" ✅ Connected! IP: " + WiFi.localIP().toString());
  else
    Serial.println(" ❌ WiFi failed.");
}

/* ================================================================
    ISD1820 VOICE ALERT
   ================================================================ */
void playVoiceAlert() {
  if (millis() - lastISDTime < ISD_COOLDOWN) return;
  Serial.println("🔊 Playing voice alert...");
  digitalWrite(ISD1820_PIN, HIGH);
  delay(300);
  digitalWrite(ISD1820_PIN, LOW);
  lastISDTime = millis();
}

/* ================================================================
    SEND TELEGRAM
   ================================================================ */
void sendTelegram(String status, String camLabel, float camConf,
                  bool flame, bool sound, bool motion, bool shock,
                  bool light, float temp, float hum) {

  if (millis() - lastTelegramTime < TELEGRAM_COOLDOWN) {
    Serial.println("⏳ Telegram cooldown active.");
    return;
  }
  if (WiFi.status() != WL_CONNECTED) { connectWiFi(); return; }

  String msg = "";
  msg += (status == "DANGEROUS") ? "🚨 *SENTINEL - DANGER DETECTED*\n"
                                 : "⚠ *SENTINEL - SUSPICIOUS ACTIVITY*\n";
  msg += "📍 Location: Room 1\n\n";
  msg += "📊 *Sensor Status:*\n";
  msg += "🔥 Flame   : " + String(flame  ? "DETECTED" : "Clear") + "\n";
  msg += "🔊 Sound   : " + String(sound  ? "DETECTED" : "Clear") + "\n";
  msg += "🏃 Motion  : " + String(motion ? "DETECTED" : "Clear") + "\n";
  msg += "💥 Shock   : " + String(shock  ? "DETECTED" : "Clear") + "\n";
  msg += "💡 Light   : " + String(light  ? "DETECTED" : "Clear") + "\n";
  msg += "🌡 Temp    : " + String(temp, 1) + "C\n";
  msg += "💧 Humidity: " + String(hum,  1) + "%\n\n";
  msg += "📷 *AI Camera Result:* " + camLabel;
  msg += " (" + String(camConf * 100.0, 1) + "%)";

  // URL encode
  msg.replace(" ",  "%20"); msg.replace("\n", "%0A");
  msg.replace("!",  "%21"); msg.replace("*",  "%2A");
  msg.replace(":",  "%3A"); msg.replace("(",  "%28");
  msg.replace(")",  "%29"); msg.replace("%",  "%25");

  String url = "https://api.telegram.org/bot" + BOT_TOKEN
             + "/sendMessage?chat_id=" + CHAT_ID
             + "&text=" + msg + "&parse_mode=Markdown";

  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  Serial.println(code == 200 ? "✅ Telegram sent!" : "❌ Telegram failed: " + String(code));
  http.end();
  lastTelegramTime = millis();
}

/* ================================================================
    READ SENSORS
   ================================================================ */
void readSensors(bool &flame, bool &sound, bool &motion, bool &shock,
                 bool &lightChange, float &temp, float &hum) {
  flame       = (digitalRead(FLAME_PIN) == LOW);
  sound       = (digitalRead(SOUND_PIN) == LOW);
  shock       = (digitalRead(SHOCK_PIN) == LOW);
  lightChange = (digitalRead(LDR_PIN)   == LOW);
  motion      = (digitalRead(PIR_PIN)   == HIGH);
  temp = dht.readTemperature();
  hum  = dht.readHumidity();
  if (isnan(temp)) temp = 0;
  if (isnan(hum))  hum  = 0;
}

/* ================================================================
    CLASSIFY SENSORS
   ================================================================ */
Situation classifySensors(bool flame, bool sound, bool motion,
                           bool shock, bool lightChange, float temp) {
  if (flame || temp >= TEMP_DANGER)  return DANGEROUS;
  int score = 0;
  if (motion)                  score++;
  if (sound)                   score++;
  if (shock)                   score++;
  if (lightChange)             score++;
  if (temp >= TEMP_SUSPICIOUS) score++;
  if (score >= 2)              return SUSPICIOUS;
  return NORMAL;
}

/* ================================================================
    SETUP
   ================================================================ */
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Sensor + ISD pins
  pinMode(FLAME_PIN,   INPUT);
  pinMode(SOUND_PIN,   INPUT);
  pinMode(PIR_PIN,     INPUT);
  pinMode(SHOCK_PIN,   INPUT);
  pinMode(LDR_PIN,     INPUT);
  pinMode(ISD1820_PIN, OUTPUT);
  digitalWrite(ISD1820_PIN, LOW);
  dht.begin();

  Serial.println("\n╔══════════════════════════════════════════════╗");
  Serial.println("║   SENTINEL — AI Incident Detection System   ║");
  Serial.println("║   Sensors + Edge Impulse AI + Telegram      ║");
  Serial.println("╚══════════════════════════════════════════════╝");

  // Init Camera
  if (ei_camera_init()) Serial.println("✅ Camera initialized");
  else                  Serial.println("❌ Camera init failed");

  // Connect WiFi
  connectWiFi();

  Serial.println("\n✅ System Ready! Monitoring started.\n");
  delay(2000);
}

/* ================================================================
    MAIN LOOP
   ================================================================ */
void loop() {
  /* ── STEP 1: Read all 6 sensors ─────────────────────────── */
  bool  flame, sound, motion, shock, lightChange;
  float temp, hum;
  readSensors(flame, sound, motion, shock, lightChange, temp, hum);
  Situation sensorSit = classifySensors(flame, sound, motion, shock, lightChange, temp);

  /* ── STEP 2: Run Edge Impulse AI inference ───────────────── */
  String cameraLabel = "Measuring...";
  float  cameraConf  = 0.0;
  Situation cameraSit = NORMAL;

  snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS *
                                   EI_CAMERA_RAW_FRAME_BUFFER_ROWS *
                                   EI_CAMERA_FRAME_BYTE_SIZE);
  if (snapshot_buf != nullptr) {
    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data     = &ei_camera_get_data;

    if (ei_camera_capture(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf)) {
      ei_impulse_result_t result = { 0 };
      EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);

      if (err == EI_IMPULSE_OK) {
        // Find label with highest confidence
        for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
          if (result.classification[i].value > cameraConf) {
            cameraConf  = result.classification[i].value;
            cameraLabel = String(ei_classifier_inferencing_categories[i]);
          }
        }
        // Map camera label to situation
        if (cameraConf >= AI_CONFIDENCE) {
          String lower = cameraLabel;
          lower.toLowerCase();
          if (lower.indexOf("danger") >= 0)   cameraSit = DANGEROUS;
          else if (lower.indexOf("sus") >= 0) cameraSit = SUSPICIOUS;
        }
      }
    }
    free(snapshot_buf);
  }

  /* ── STEP 3: Combined Decision ───────────────────────────── */
  // Take highest threat level from sensors OR camera
  Situation finalSit = (sensorSit >= cameraSit) ? sensorSit : cameraSit;

  /* ── STEP 4: Print to Serial Monitor ────────────────────── */
  Serial.println("\n┌──────────────────────────────────────────────┐");
  Serial.println("│              SENTINEL STATUS                 │");
  Serial.println("└──────────────────────────────────────────────┘");
  Serial.print("  🔥 Flame       : "); Serial.println(flame       ? "DETECTED ⚠" : "Clear");
  Serial.print("  🔊 Sound       : "); Serial.println(sound       ? "DETECTED ⚠" : "Clear");
  Serial.print("  🏃 Motion(PIR) : "); Serial.println(motion      ? "DETECTED ⚠" : "Clear");
  Serial.print("  💥 Shock       : "); Serial.println(shock       ? "DETECTED ⚠" : "Clear");
  Serial.print("  💡 LightChange : "); Serial.println(lightChange ? "DETECTED ⚠" : "Clear");
  Serial.print("  🌡 Temperature : "); Serial.print(temp,1); Serial.print(" C  | Humidity: "); Serial.print(hum,1); Serial.println(" %");
  Serial.print("  📷 Camera AI   : "); Serial.print(cameraLabel); Serial.print("  Confidence: "); Serial.print(cameraConf*100,1); Serial.println("%");

  Serial.println("┌──────────────────────────────────────────────┐");
  Serial.print("│  Sensor → ");
  switch (sensorSit) {
    case NORMAL:     Serial.println("NORMAL                           │"); break;
    case SUSPICIOUS: Serial.println("SUSPICIOUS                       │"); break;
    case DANGEROUS:  Serial.println("DANGEROUS                        │"); break;
  }
  Serial.print("│  Camera → ");
  switch (cameraSit) {
    case NORMAL:     Serial.println("NORMAL                           │"); break;
    case SUSPICIOUS: Serial.println("SUSPICIOUS                       │"); break;
    case DANGEROUS:  Serial.println("DANGEROUS                        │"); break;
  }
  Serial.println("│──────────────────────────────────────────────│");
  switch (finalSit) {
    case NORMAL:
      Serial.println("│  ✅ FINAL : NORMAL — All Clear               │"); break;
    case SUSPICIOUS:
      Serial.println("│  ⚠  FINAL : SUSPICIOUS ACTIVITY DETECTED     │"); break;
    case DANGEROUS:
      Serial.println("│  🚨 FINAL : DANGER! EMERGENCY DETECTED       │"); break;
  }
  Serial.println("└──────────────────────────────────────────────┘");

  /* ── STEP 5: Trigger Alerts ─────────────────────────────── */
  if (finalSit == DANGEROUS) {
    playVoiceAlert();
    sendTelegram("DANGEROUS", cameraLabel, cameraConf,
                 flame, sound, motion, shock, lightChange, temp, hum);
  } else if (finalSit == SUSPICIOUS) {
    playVoiceAlert();
    sendTelegram("SUSPICIOUS", cameraLabel, cameraConf,
                 flame, sound, motion, shock, lightChange, temp, hum);
  }

  ei_sleep(2000);
}

/* ================================================================
    CAMERA FUNCTIONS (Edge Impulse)
   ================================================================ */
bool ei_camera_init(void) {
  if (is_initialised) return true;
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed 0x%x\n", err);
    return false;
  }
  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, 0);
  }
  is_initialised = true;
  return true;
}

void ei_camera_deinit(void) {
  esp_err_t err = esp_camera_deinit();
  if (err != ESP_OK) { ei_printf("Camera deinit failed\n"); return; }
  is_initialised = false;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
  if (!is_initialised) { ei_printf("Camera not initialized\r\n"); return false; }
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { ei_printf("Camera capture failed\n"); return false; }
  bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
  esp_camera_fb_return(fb);
  if (!converted) { ei_printf("Conversion failed\n"); return false; }
  if (img_width  != EI_CAMERA_RAW_FRAME_BUFFER_COLS ||
      img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS) {
    ei::image::processing::crop_and_interpolate_rgb888(
        out_buf, EI_CAMERA_RAW_FRAME_BUFFER_COLS, EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf, img_width, img_height);
  }
  return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
  size_t pixel_ix   = offset * 3;
  size_t pixels_left = length;
  size_t out_ptr_ix  = 0;
  while (pixels_left != 0) {
    out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16)
                        + (snapshot_buf[pixel_ix + 1] << 8)
                        +  snapshot_buf[pixel_ix];
    out_ptr_ix++;
    pixel_ix += 3;
    pixels_left--;
  }
  return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif
