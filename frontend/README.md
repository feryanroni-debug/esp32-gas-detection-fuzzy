# Frontend Web Dashboard - Sistem Deteksi Kebocoran Gas Real-Time

Antarmuka web modern responsif untuk pemantauan parameter konsentrasi gas LPG, asap pembakaran, deteksi nyala api inframerah, dan status inferensi Fuzzy Logic Mamdani secara *real-time*.

## Fitur Utama
1. **4 Kartu Sensor Telemetri Real-Time**:
   - Gas LPG (MQ-2 dalam ppm) + Bar Progres.
   - Konsentrasi Asap (MQ-2 dalam ppm).
   - Deteksi Api (Flame IR Sensor).
   - Suhu Ruangan (°C).
2. **Banner Status Fuzzy Terpusat**:
   - Indikator status dinamis: **AMAN** (Hijau), **WASPADA** (Kuning), atau **BAHAYA** (Merah).
   - Menampilkan nilai crisp $K$ (0–100).
   - Efek animasi denyut (*pulsing*) saat kondisi bahaya.
3. **Peringatan Audio Web**:
   - Menggunakan *Web Audio API* untuk membunyikan sirine bahaya secara otomatis di browser saat status BAHAYA.
   - Dilengkapi tombol *Mute Alarm* (Ack).
4. **Grafik Tren Waktu Nyata**:
   - Multi-line chart menggunakan Chart.js untuk memantau tren 20 pembacaan terakhir.
5. **Riwayat & Audit Log**:
   - Halaman `history.html` untuk melihat daftar insiden alarm dan log telemetri sensor.
6. **Autentikasi Petugas**:
   - Halaman `login.html` untuk membatasi akses petugas HSE / Operator.

## Cara Menjalankan
Halaman ini disajikan secara otomatis oleh server backend di `http://localhost:5000` atau dapat dibuka langsung melalui web server statis / peramban web modern.
