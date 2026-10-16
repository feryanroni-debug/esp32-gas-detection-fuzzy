# Backend Laravel 13 & MySQL - Sistem Monitoring Deteksi Gas & Api

Aplikasi backend berbasis **Laravel 13** dan **MySQL** untuk menerima telemetri dari mikrokontroler ESP32, memproses logging kejadian bahaya (alarm events), dan menyajikan Web Monitoring Dashboard real-time.

---

## 🏗️ Arsitektur & Struktur Proyek

```text
backend_laravel/
├── app/
│   ├── Http/
│   │   ├── Controllers/
│   │   │   ├── TelemetryController.php     # Receiver API data ESP32 & query riwayat
│   │   │   └── DashboardController.php     # Controller view monitoring & riwayat
│   │   └── Middleware/
│   │       └── VerifyEsp32ApiKey.php       # Validasi header X-API-KEY dari ESP32
│   └── Models/
│       ├── SensorTelemetry.php             # Eloquent Model tabel sensor_telemetries
│       └── AlarmEvent.php                  # Eloquent Model tabel alarm_events
├── database/
│   └── migrations/
│       ├── 2026_10_16_000001_create_sensor_telemetries_table.php
│       └── 2026_10_16_000002_create_alarm_events_table.php
├── resources/
│   └── views/
│       ├── dashboard.blade.php             # Dashboard monitoring real-time (Tailwind + Chart.js)
│       └── history.blade.php               # Halaman log riwayat insiden bahaya
├── routes/
│   ├── api.php                             # Definisi rute REST API ESP32
│   └── web.php                             # Definisi rute web dashboard
└── .env                                    # Konfigurasi koneksi MySQL & API Key
```

---

## ⚙️ Konfigurasi Database MySQL (.env)

Konfigurasi koneksi database MySQL pada berkas `.env`:

```env
DB_CONNECTION=mysql
DB_HOST=127.0.0.1
DB_PORT=3306
DB_DATABASE=gas_detection_db
DB_USERNAME=root
DB_PASSWORD=

ESP32_API_KEY=secret_esp32_gas_token_2026
```

---

## 🚀 Cara Menjalankan

### 1. Aktifkan MySQL Service
Anda dapat mengaktifkan MySQL melalui salah satu cara berikut:
- **Melalui Laravel Herd**:
  1. Buka aplikasi **Laravel Herd** dari menu Windows.
  2. Buka tab **Services** di panel samping.
  3. Klik **Add Service** $\rightarrow$ pilih **MySQL**, lalu klik **Start**.
- **Melalui XAMPP / MariaDB**:
  - Buka XAMPP Control Panel dan klik tombol **Start** pada modul **MySQL**.

### 2. Jalankan Migrasi Database
Jalankan perintah migrasi untuk membuat tabel `sensor_telemetries` dan `alarm_events`:
```bash
php artisan migrate
```

### 3. Jalankan Server Laravel
```bash
php artisan serve --port=8000
```
Buka peramban di [http://localhost:8000](http://localhost:8000).

---

## 📡 Dokumentasi Endpoint REST API

| Endpoint | Method | Header / Auth | Deskripsi |
| :--- | :---: | :--- | :--- |
| `/api/health` | `GET` | Publik | Health check status Laravel 13 & koneksi DB |
| `/api/v1/telemetry` | `POST` | `X-API-KEY: secret_esp32_gas_token_2026` | Ingest data telemetri dari mikrokontroler ESP32 |
| `/api/v1/telemetry/latest` | `GET` | Publik | Mendapatkan pembacaan telemetri paling terkini |
| `/api/v1/telemetry/history` | `GET` | Publik | Riwayat pembacaan sensor untuk grafik Chart.js |
| `/api/v1/telemetry/alarms` | `GET` | Publik | Riwayat daftar kejadian alarm (Waspada/Bahaya) |
