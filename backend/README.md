# Backend & Real-Time Server - Sistem Deteksi Kebocoran Gas

Server backend berbasis **Node.js, Express.js, Socket.IO, dan SQLite** untuk menerima data telemetri dari mikrokontroler ESP32, menyiarkan pembaruan data secara *real-time* ke browser, serta mencatat data log dan riwayat insiden alarm.

## Fitur Utama
- **Ingestion API**: `POST /api/v1/telemetry` dengan validasi `X-API-KEY`.
- **Real-Time Streaming**: Socket.IO event `sensor_update` dengan latensi $< 500$ ms.
- **Autentikasi Pengguna**: Login & Registrasi dengan enkripsi `bcrypt` dan otorisasi JWT.
- **Penyimpanan Log**: Skema database relasional otomatis (`sensor_telemetry`, `alarm_events`, `users`).
- **Static Hosting**: Langsung melayani halaman web dashboard di root URL (`http://localhost:5000`).

## Persyaratan
- Node.js (v18+ LTS direkomendasikan)
- npm

## Panduan Instalasi & Menjalankan

1. Salin template variabel lingkungan:
   ```bash
   cp .env.example .env
   ```
2. Instal dependensi:
   ```bash
   npm install
   ```
3. Jalankan server:
   ```bash
   # Mode Development
   npm run dev

   # Mode Produksi
   npm start
   ```
4. Buka peramban di `http://localhost:5000`.
