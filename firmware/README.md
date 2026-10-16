# Firmware ESP32 - Sistem Deteksi Kebocoran Gas, Asap, dan Api

Folder ini berisi kode program mikrokontroler **ESP32 DOIT DevKit V1** yang menjalankan algoritma **Fuzzy Logic Mamdani (27 Rules)** langsung pada chip (*edge computing*), pembacaan sensor multi-sampling, kontrol LCD 16x2 I2C, alarm Buzzer, serta pengiriman data telemetri real-time via Wi-Fi.

## Struktur Berkas
- `esp32_gas_detection/`
  - `esp32_gas_detection.ino`: Program utama (setup, loop, koneksi Wi-Fi, pengiriman JSON HTTP/WebSocket).
  - `config.h`: Definisi pinout GPIO, kredensial Wi-Fi, interval sampling, dan endpoint API backend.
  - `FuzzyEngine.h`: Definisi kelas dan fungsi keanggotaan Mamdani (Aman, Waspada, Bahaya).
  - `FuzzyEngine.cpp`: Implementasi evaluasi 27 aturan dan defuzzifikasi Centroid.

## Wiring Pinout Hardware
| Komponen | Pin ESP32 | Keterangan |
| :--- | :--- | :--- |
| **MQ-2 (Gas LPG)** | GPIO 34 (ADC1_CH6) | Sinyal Analog (0–4095) |
| **MQ-2 (Asap)** | GPIO 35 (ADC1_CH7) | Sinyal Analog (0–4095) |
| **Flame Sensor** | GPIO 32 (Analog) & GPIO 33 (Digital) | Deteksi IR Api |
| **DHT11 / DHT22** | GPIO 4 | Sensor Suhu Lingkungan |
| **LCD 16x2 I2C** | SDA: GPIO 21, SCL: GPIO 22 | Layar display lokal (0x27 / 0x3F) |
| **Active Buzzer** | GPIO 19 | Alarm lokal (via transistor NPN) |

## Dependensi Library Arduino
- `ArduinoJson` (v6 atau v7)
- `LiquidCrystal_I2C`
- `DHT sensor library` (jika menggunakan DHT11/22)
- `WiFi` & `HTTPClient` (bawaan board ESP32)
