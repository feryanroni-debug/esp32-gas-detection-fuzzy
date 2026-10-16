# Sistem Deteksi Kebocoran Gas, Asap, dan Api Berbasis ESP32 dengan Fuzzy Logic Mamdani & Web Monitoring Real-Time

Repositori ini berisi implementasi menyeluruh sistem Internet of Things (IoT) untuk pemantauan dini bahaya kebocoran gas LPG, konsentrasi asap, dan nyala api berbasis **ESP32**, **Fuzzy Logic Mamdani (27 Rules)**, dan **Web Monitoring Dashboard**.

Dokumen acuan teknis: [PRD_Sistem_Deteksi_Kebocoran_Gas.md](PRD_Sistem_Deteksi_Kebocoran_Gas.md)

---

## 📁 Struktur Repositori Modular

```text
├── PRD_Sistem_Deteksi_Kebocoran_Gas.md  # Dokumen Product Requirement (PRD)
├── README.md                           # Dokumentasi umum repositori
├── .gitignore                          # Konfigurasi file yang diabaikan git
│
├── firmware/                           # Subsistem Mikrokontroler & Edge Computing
│   ├── esp32_gas_detection/
│   │   ├── esp32_gas_detection.ino     # Sketch utama ESP32
│   │   ├── config.h                    # Konfigurasi Wi-Fi, pinout & endpoint API
│   │   ├── FuzzyEngine.h               # Deklarasi kelas FIS Mamdani
│   │   └── FuzzyEngine.cpp             # Implementasi fungsi keanggotaan & 27 aturan
│   └── README.md                       # Petunjuk kompilasi & wiring ESP32
│
├── backend_laravel/                    # Subsistem Backend API Laravel 13 & MySQL
│   ├── app/Http/Controllers/           # TelemetryController & DashboardController
│   ├── app/Http/Middleware/            # VerifyEsp32ApiKey middleware
│   ├── app/Models/                     # SensorTelemetry & AlarmEvent Eloquent Models
│   ├── database/migrations/            # Migrasi tabel MySQL sensor_telemetries & alarm_events
│   ├── resources/views/                # Blade View Dashboard & History
│   ├── routes/api.php & web.php        # Routing REST API & Web Dashboard
│   └── README.md                       # Petunjuk konfigurasi & migrasi MySQL
│
├── backend/                            # Subsistem Alternatif Node.js & Socket.IO
│   ├── server.js                       # Entry point HTTP server & Socket.IO
│   ├── package.json                    # Dependensi Node.js
│   ├── database/                       # Skema database & file JSON persisten
│   └── src/                            # Controller, routes, & socket handler
│
└── frontend/                           # Subsistem Antarmuka Web Dashboard Statis
    ├── index.html                      # Dashboard monitoring kartu telemetri real-time
    ├── login.html                      # Halaman autentikasi pengguna
    ├── history.html                    # Halaman tabel riwayat telemetri & insiden
    ├── css/                            # Styling kustom & tema antarmuka
    ├── js/                             # Logika frontend, koneksi Socket.IO & Chart.js
    └── README.md                       # Petunjuk penggunaan antarmuka web
```

---

## 🚀 Panduan Memulai Cepat

1. **Firmware ESP32**: Buka folder `firmware/esp32_gas_detection/` menggunakan Arduino IDE atau PlatformIO. Sesuaikan kredensial Wi-Fi dan IP server di `config.h`.
2. **Backend**: Masuk ke direktori `backend/`, buat file `.env` dari `.env.example`, jalankan `npm install`, lalu `npm run dev`.
3. **Frontend**: Buka `frontend/index.html` langsung di browser atau host bersama server Express.

---
*Proyek Akhir Sarjana Terapan PJJ Teknik Telekomunikasi PENS 2026 - Feryan Hidayat Roni Wahyudi*
