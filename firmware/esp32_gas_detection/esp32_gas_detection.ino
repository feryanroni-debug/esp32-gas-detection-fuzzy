#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#include "config.h"
#include "FuzzyEngine.h"

// Inisialisasi Objek
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);
FuzzyEngine fuzzyEngine;

// Variabel Waktu (Non-blocking timer)
unsigned long lastFuzzyLcdMs = 0;
unsigned long lastTelemetryMs = 0;
unsigned long lastBuzzerToggleMs = 0;
unsigned long lastWifiCheckMs = 0;

bool buzzerState = false;
FuzzyResult currentFuzzyResult = {0.0f, CONDITION_AMAN, "AMAN", 0.0f, 0.0f, 0.0f};

// Variabel Bacaan Sensor
float currentGasPpm = 0.0f;
float currentSmokePpm = 0.0f;
float currentFlameNorm = 0.0f;
String currentFlameStatus = "TIDAK_ADA";
float currentTemperature = 28.5f; // Nilai default suhu ruangan jika DHT tidak aktif

// ==========================================
// FUNGSI SAMPLING SENSOR (MULTI-SAMPLING ADC)
// ==========================================
float readAnalogAverage(int pin, int samples) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(pin);
        delayMicroseconds(200);
    }
    return (float)sum / samples;
}

void readAllSensors() {
    // 1. Baca MQ-2 Gas (ADC 12-bit: 0 - 4095)
    float rawGas = readAnalogAverage(PIN_MQ2_GAS, SAMPLE_COUNT);
    // Konversi pendekatan kurva voltase ke ppm (skala 0 - 5000 ppm)
    currentGasPpm = (rawGas / 4095.0f) * 5000.0f;

    // 2. Baca MQ-2 Asap (ADC 12-bit: 0 - 4095)
    float rawSmoke = readAnalogAverage(PIN_MQ2_SMOKE, SAMPLE_COUNT);
    // Konversi pendekatan ke ppm (skala 0 - 1000 ppm)
    currentSmokePpm = (rawSmoke / 4095.0f) * 1000.0f;

    // 3. Baca Flame Sensor (Active LOW pada modul umum, dinormalisasi ke 0.0 - 1.0)
    float rawFlame = readAnalogAverage(PIN_FLAME_ANALOG, SAMPLE_COUNT);
    // Inversi nilai analog agar api semakin terdeteksi = nilai mendekati 1.0
    currentFlameNorm = 1.0f - (rawFlame / 4095.0f);
    if (currentFlameNorm < 0.0f) currentFlameNorm = 0.0f;
    if (currentFlameNorm > 1.0f) currentFlameNorm = 1.0f;

    if (currentFlameNorm < 0.25f) {
        currentFlameStatus = "TIDAK_ADA";
    } else if (currentFlameNorm < 0.6f) {
        currentFlameStatus = "TERDETEKSI";
    } else {
        currentFlameStatus = "BESAR";
    }
}

// ==========================================
// KENDALI BUZZER LOKAL
// ==========================================
void handleBuzzer() {
    unsigned long now = millis();
    if (currentFuzzyResult.condition == CONDITION_AMAN) {
        digitalWrite(PIN_BUZZER, LOW);
        buzzerState = false;
    } else if (currentFuzzyResult.condition == CONDITION_WASPADA) {
        // Nada intermiten bip (500 ms ON, 500 ms OFF)
        if (now - lastBuzzerToggleMs >= 500) {
            lastBuzzerToggleMs = now;
            buzzerState = !buzzerState;
            digitalWrite(PIN_BUZZER, buzzerState ? HIGH : LOW);
        }
    } else { // CONDITION_BAHAYA
        // Nada kontinu tanpa jeda
        digitalWrite(PIN_BUZZER, HIGH);
        buzzerState = true;
    }
}

// ==========================================
// PEMBARUAN DISPLAY LCD 16x2
// ==========================================
void updateLcd() {
    // Baris 1: Nilai Gas & Asap
    lcd.setCursor(0, 0);
    lcd.print("G:");
    lcd.print((int)currentGasPpm);
    lcd.print(" S:");
    lcd.print((int)currentSmokePpm);
    lcd.print("   "); // Clears leftover chars

    // Baris 2: Status Fuzzy & Api
    lcd.setCursor(0, 1);
    lcd.print("Sts:");
    lcd.print(currentFuzzyResult.conditionStr);
    lcd.print(" F:");
    if (currentFlameNorm >= 0.25f) {
        lcd.print("FIRE!");
    } else {
        lcd.print("OK   ");
    }
}

// ==========================================
// PENGIRIMAN TELEMETRI KE BACKEND
// ==========================================
void sendTelemetry() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Wi-Fi] Belum terhubung. Lewati pengiriman telemetri.");
        return;
    }

    HTTPClient http;
    String endpointUrl = String("http://") + BACKEND_HOST + ":" + BACKEND_PORT + API_TELEMETRY_PATH;

    http.begin(endpointUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-KEY", API_KEY_SECRET);
    http.setTimeout(2000);

    // Bangun JSON payload
    StaticJsonDocument<384> doc;
    doc["deviceId"] = DEVICE_ID;
    doc["gasPpm"] = round(currentGasPpm * 10.0f) / 10.0f;
    doc["smokePpm"] = round(currentSmokePpm * 10.0f) / 10.0f;
    doc["flameRaw"] = round(currentFlameNorm * 100.0f) / 100.0f;
    doc["flameStatus"] = currentFlameStatus;
    doc["temperature"] = currentTemperature;
    doc["fuzzyCrisp"] = round(currentFuzzyResult.crispScore * 10.0f) / 10.0f;
    doc["fuzzyStatus"] = currentFuzzyResult.conditionStr;
    doc["buzzerActive"] = buzzerState;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpCode = http.POST(requestBody);
    if (httpCode > 0) {
        Serial.printf("[HTTP POST] Sukses (%d)\n", httpCode);
    } else {
        Serial.printf("[HTTP POST] Gagal kirim: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

// ==========================================
// SETUP & LOOP UTAMA
// ==========================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== Inisialisasi Sistem Deteksi Gas ESP32 ===");

    // Inisialisasi Pinout
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    pinMode(PIN_FLAME_DIGITAL, INPUT);

    // Inisialisasi LCD
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ESP32 Gas System");
    lcd.setCursor(0, 1);
    lcd.print("Connecting WiFi.");

    // Koneksi Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Menghubungkan ke Wi-Fi: ");
    Serial.println(WIFI_SSID);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[Wi-Fi] Terhubung! IP: " + WiFi.localIP().toString());
        lcd.setCursor(0, 1);
        lcd.print("WiFi Connected! ");
    } else {
        Serial.println("\n[Wi-Fi] Gagal konek awal. Sistem tetap jalan secara lokal.");
        lcd.setCursor(0, 1);
        lcd.print("WiFi Offline    ");
    }
    delay(1000);
    lcd.clear();
}

void loop() {
    unsigned long now = millis();

    // 1. Eksekusi Inferensi Fuzzy & Update LCD setiap 1 detik
    if (now - lastFuzzyLcdMs >= LOOP_INTERVAL_MS) {
        lastFuzzyLcdMs = now;

        readAllSensors();
        currentFuzzyResult = fuzzyEngine.compute(currentGasPpm, currentSmokePpm, currentFlameNorm);
        updateLcd();

        // Serial log untuk debugging
        Serial.printf("[DATA] Gas: %.1f ppm | Asap: %.1f ppm | Api: %.2f (%s) => Fuzzy: %.2f [%s]\n",
            currentGasPpm, currentSmokePpm, currentFlameNorm,
            currentFlameStatus.c_str(), currentFuzzyResult.crispScore, currentFuzzyResult.conditionStr);
    }

    // 2. Kendali Alarm Buzzer (Real-time)
    handleBuzzer();

    // 3. Pengiriman Telemetri (Tiap 2.5s atau seketika jika Bahaya)
    if (now - lastTelemetryMs >= TELEMETRY_INTERVAL || (currentFuzzyResult.condition == CONDITION_BAHAYA && (now - lastTelemetryMs >= 1000))) {
        lastTelemetryMs = now;
        sendTelemetry();
    }

    // 4. Pemeriksaan Rekoneksi Wi-Fi di latar belakang
    if (now - lastWifiCheckMs >= WIFI_RECONNECT_MS) {
        lastWifiCheckMs = now;
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[Wi-Fi] Mencoba menyambung kembali...");
            WiFi.reconnect();
        }
    }
}
