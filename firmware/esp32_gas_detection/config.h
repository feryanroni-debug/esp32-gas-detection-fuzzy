#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// KONFIGURASI JARINGAN WI-FI
// ==========================================
#define WIFI_SSID           "NAMA_WIFI_ANDA"
#define WIFI_PASSWORD       "PASSWORD_WIFI"
#define WIFI_RECONNECT_MS   10000

// ==========================================
// KONFIGURASI BACKEND SERVER & API
// ==========================================
#define BACKEND_HOST        "192.168.1.100"  // Sesuaikan dengan IP Server Backend
#define BACKEND_PORT        5050
#define API_TELEMETRY_PATH  "/api/v1/telemetry"
#define DEVICE_ID           "ESP32-GAS-01"
#define API_KEY_SECRET      "secret_esp32_gas_token_2026"

// ==========================================
// KONFIGURASI PINOUT HARDWARE (ESP32)
// ==========================================
#define PIN_MQ2_GAS         34  // ADC1_CH6 (Analog)
#define PIN_MQ2_SMOKE       35  // ADC1_CH7 (Analog)
#define PIN_FLAME_ANALOG    32  // ADC1_CH4 (Analog)
#define PIN_FLAME_DIGITAL   33  // Digital Input
#define PIN_DHT             4   // DHT11/DHT22 (Digital Data)
#define PIN_BUZZER          19  // Active Buzzer 5V via NPN Transistor
#define PIN_I2C_SDA         21  // LCD I2C SDA
#define PIN_I2C_SCL         22  // LCD I2C SCL

#define LCD_I2C_ADDRESS     0x27 // Atau 0x3F tergantung modul backpack PCF8574
#define LCD_COLS            16
#define LCD_ROWS            2

// ==========================================
// INTERVAL & PARAMETER SAMPLING
// ==========================================
#define SAMPLE_COUNT        10   // Multi-sampling derau ADC
#define LOOP_INTERVAL_MS    1000 // Siklus inferensi fuzzy & display lokal (1 detik)
#define TELEMETRY_INTERVAL  2500 // Interval kirim normal ke backend (2.5 detik)

#endif // CONFIG_H
