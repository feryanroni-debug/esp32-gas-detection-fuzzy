# PRODUCT REQUIREMENT DOCUMENT (PRD)

## SISTEM DETEKSI KEBOCORAN GAS, ASAP, DAN API BERBASIS ESP32 MENGGUNAKAN METODE FUZZY LOGIC MAMDANI DAN WEB MONITORING REAL-TIME

---

### Informasi Dokumen
- **Judul Proyek**: Rancang Bangun Sistem Deteksi Kebocoran Gas Menggunakan Metode Fuzzy Logic
- **Penyusun**: Feryan Hidayat Roni Wahyudi (NRP: 2223612201)
- **Program Studi**: Sarjana Terapan PJJ Teknik Telekomunikasi, Jurusan Teknik Elektro
- **Institusi**: Politeknik Elektronika Negeri Surabaya (PENS)
- **Tahun**: 2026
- **Dosen Pembimbing**:
  1. Nihayatus Sa’adah, S.ST., M.T. (NIP: 199410172020122019)
  2. Karimatun Nisa’, S.T., M.T. (NIP: 199508272022032011)
- **Versi Dokumen**: v1.0.0
- **Status Dokumen**: Approved for Implementation

---

## 1. EXECUTIVE SUMMARY & LATAR BELAKANG

### 1.1 Ringkasan Eksekutif
Sistem Deteksi Kebocoran Gas Berbasis Fuzzy Logic adalah sebuah solusi *Internet of Things* (IoT) terintegrasi yang dirancang untuk mendeteksi risiko kebocoran gas LPG (*Liquefied Petroleum Gas*), akumulasi asap pembakaran, serta munculnya nyala api secara dini di area industri maupun komersial/domestik. 

Sistem ini memadukan mikrokontroler **ESP32** sebagai pusat kendali komputasi cerdas di tepi jaringan (*edge computing*), sensor gas & asap **MQ-2**, dan **Flame Sensor** inframerah. Algoritma inferensi **Fuzzy Logic Mamdani** (27 aturan) diimplementasikan secara langsung pada ESP32 untuk memetakan tingkat bahaya menjadi tiga kategori: **Aman**, **Waspada**, dan **Bahaya**. Informasi kondisi disajikan secara lokal melalui **LCD 16x2 I2C** dan alarm **Buzzer**, serta ditransmisikan secara *real-time* via Wi-Fi ke **Web Monitoring Dashboard** yang dilengkapi sistem autentikasi, logging database, grafik tren historis, dan peringatan dini instan dengan latensi transmisi < 2 detik.

### 1.2 Masalah yang Diselesaikan (Problem Statement)
1. **Kelemahan Deteksi Reaktif & Manual**: Sebagian besar sistem deteksi konvensional masih mengandalkan ambang batas kaku (*crisp thresholding*) satu sensor atau prosedur manual *stop-call-wait*, yang sering kali terlambat memberikan peringatan sebelum akumulasi gas mencapai batas mudah terbakar (*Lower Explosive Limit* / LEL 1,8%–2,1%).
2. **Tingginya Tingkat Ketidakpastian Lingkungan**: Konsentrasi gas dan asap di udara mengalami fluktuasi akibat ventilasi dan suhu. Sistem tanpa kecerdasan buatan rentan terhadap *false alarm* (alarm palsu) atau justru tidak mendeteksi anomali bertahap.
3. **Ketiadaan Monitoring Terintegrasi Jarak Jauh**: Banyak instalasi alarm hanya memiliki indikator lokal (sirine/lampu). Operator atau tim keselamatan (K3) di luar ruangan tidak dapat memantau parameter fisik secara langsung dan historis.
4. **Ketergantungan Layanan Pihak Ketiga**: Beberapa penelitian sebelumnya mengandalkan platform pihak ketiga (seperti Blynk/Thingspeak) yang memiliki pembatasan *rate limit*, ketergantungan server eksternal, dan kurang fleksibel untuk kebutuhan integrasi industri terpusat.

### 1.3 Nilai Kebaruan & Research Gap
Berdasarkan tinjauan pustaka terhadap penelitian terdahulu (Maulana 2024, Arif 2024, Fahreza 2024, Surya 2026, Fauzi 2025):
- Menggabungkan **tiga parameter multi-sensor** (Gas LPG, Konsentrasi Asap, dan Deteksi Nyala Api) dalam satu proses inferensi simultan.
- Menerapkan **Fuzzy Logic Mamdani 27 Rules** langsung di mikroprosesor ESP32 (*on-chip inference*) untuk klasifikasi dinamis Aman–Waspada–Bahaya tanpa ketergantungan koneksi komputasi cloud.
- Mengembangkan **Web Monitoring Mandiri (*Self-hosted / Dedicated Web Dashboard*)** yang dilengkapi fitur autentikasi (login/register), penyimpanan data terstruktur (*database logging*), visualisasi kartu sensor *real-time*, dan notifikasi audio-visual di peramban web.

---

## 2. USER PERSONA & USE CASES

### 2.1 Persona Pengguna
1. **Operator Lapangan / Dapur / Fasilitas**:
   - *Karakteristik*: Berada di sekitar area fisik instalasi gas.
   - *Kebutuhan*: Indikator lokal yang langsung terbaca (LCD 16x2) dan alarm suara keras (Buzzer) yang membedakan kondisi Waspada vs Bahaya secara instan.
2. **Teknisi K3 / Maintenance (HSE Officer)**:
   - *Karakteristik*: Bertanggung jawab atas keselamatan area kerja dan perawatan preventif.
   - *Kebutuhan*: Web dashboard *real-time*, grafik tren konsentrasi gas/asap, riwayat kejadian (*event log*), dan notifikasi bahaya jarak jauh pada smartphone atau laptop.
3. **Superadmin / Manajer Fasilitas**:
   - *Karakteristik*: Pengelola hak akses dan audit berkala.
   - *Kebutuhan*: Autentikasi user terenkripsi, ekspor laporan log insiden kebocoran, serta pengaturan parameter ambang batas.

### 2.2 Skenario Penggunaan Utama (Use Cases)
```
[Sensor MQ-2 & Flame] ──> [ESP32: Fuzzy Mamdani] ──> [LCD 16x2 & Buzzer Lokal]
                                    │
                                (Wi-Fi)
                                    ▼
                         [Backend REST API / WS]
                                    │
                         ┌──────────┴──────────┐
                         ▼                     ▼
                  [Database Logs]     [Web Dashboard Real-Time]
                                      (Operator / HSE / Admin)
```

1. **UC-01 (Normal Monitoring)**: Sensor membaca kondisi udara normal. ESP32 menghitung nilai fuzzy output $K < 25$. LCD menampilkan "Status: Aman", Buzzer MATI, Web Dashboard menampilkan kartu serba hijau (*Normal/Safe*), data disimpan ke database periodik.
2. **UC-02 (Deteksi Kebocoran Gas Awal / Pre-hazard)**: Konsentrasi gas naik ke 800 ppm, asap tipis 250 ppm, tidak ada api. Hasil inferensi menghasilkan status **Waspada**. LCD menampilkan "Status: Waspada", Buzzer berbunyi intermiten (bip berkala), Web Dashboard berubah menjadi warna kuning/peringatan. Teknisi K3 segera memeriksa regulator sebelum gas terakumulasi.
3. **UC-03 (Kondisi Bahaya / Kebocoran Kritis & Api)**: Gas > 1500 ppm atau nyala api terdeteksi. Hasil fuzzy menghasilkan status **Bahaya** ($K \ge 50$). ESP32 menyalakan Buzzer secara kontinu (alarm darurat), LCD menampilkan peringatan bahaya, Web Dashboard memicu alert merah berkedip, sirene audio di browser aktif. Prosedur evakuasi dan penutupan katup gas dilakukan seketika.
4. **UC-04 (Audit & Analisis Insiden)**: HSE Officer membuka menu riwayat laporan web untuk mengevaluasi jam terjadinya kebocoran, durasi kejadian, dan pola kenaikan gas sebelum tindakan diambil.

---

## 3. ARSITEKTUR TEKNOLOGI & REKOMENDASI STACK

Sistem dibangun menggunakan pemisahan lapisan (*layered architecture*) yang modular:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        LAPISAN HARDWARE (EDGE)                         │
│  - Sensor MQ-2 (Gas LPG) [GPIO34/ADC1]                                 │
│  - Sensor MQ-2 (Asap) [GPIO35/ADC1]                                    │
│  - Flame Sensor (Api) [GPIO32/ADC1 & GPIO33/Digital]                   │
│  - Sensor Suhu Lingkungan (DHT11/DHT22) [GPIO4/Digital] (Opsional)     │
│  - LCD 16x2 + I2C Backpack (PCF8574) [SDA: GPIO21, SCL: GPIO22]        │
│  - Active Buzzer 5V [GPIO19]                                           │
│  - Unit Pemroses Utama: ESP32 DOIT DevKit V1 (Tensilica Xtensa Dual-Core) │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                        HTTP POST JSON / WebSocket
                               (Wi-Fi 2.4 GHz)
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                       LAPISAN BACKEND & DATABASE                       │
│  - Runtime: Node.js (v18+ LTS) / Express.js atau Python 3.11 (FastAPI) │
│  - Real-Time Broker: Socket.IO / WebSockets                            │
│  - Database: PostgreSQL / MySQL / SQLite (Development)                 │
│  - Security: JWT Authentication, bcrypt, Helmet, CORS Protection       │
│  - Endpoints: Ingest Telemetri, Auth User, Query Log Historis          │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
                         WebSocket Push / REST API
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                    LAPISAN FRONTEND (WEB DASHBOARD)                    │
│  - Struktur & Tampilan: HTML5, Modern CSS / Tailwind CSS               │
│  - Interaktivitas: JavaScript Modern (Vanilla JS / React + Vite)       │
│  - Visualisasi Grafik: Chart.js / ApexCharts (Time-series real-time)   │
│  - Alerting Audio: Web Audio API / HTML5 Audio Beep Alarm              │
│  - Desain Kartu: Sesuai Gambar 3.2.3 Proposal (Gas, Suhu, Asap, Api)   │
└────────────────────────────────────────────────────────────────────────┘
```

### 3.1 Justifikasi Pemilihan Bahasa Pemrograman & Pustaka

#### 1. Firmware ESP32: C/C++ (Arduino Core / PlatformIO)
- **Alasan**: Memberikan latensi paling minim untuk eksekusi algoritma Fuzzy Logic Mamdani secara kontinu. 
- **Pustaka Kunci**:
  - `WiFi.h` & `HTTPClient.h` / `WebSocketsClient.h`: Komunikasi jaringan Wi-Fi.
  - `LiquidCrystal_I2C.h`: Komunikasi layar LCD via bus $I^2C$.
  - `ArduinoJson.h` (v6/v7): Serialisasi data sensor ke payload JSON secara efisien dan hemat memori.
  - Modul Algoritma Mamdani Mandiri (dapat dikembangkan dalam berkas C++ terpisah `FuzzyEngine.h` dan `FuzzyEngine.cpp` untuk transparansi akademis perhitungan rumus keanggotaan).

#### 2. Backend Server: Node.js (Express.js + Socket.IO) atau Python (FastAPI)
- **Node.js (Express.js + Socket.IO)** (*Pilihan Utama*):
  - Model I/O non-blocking berbasis *event-driven* sangat ideal untuk menerima *stream* data telemetri ESP32 dan menyiarkannya (*broadcast*) secara *sub-second* ke seluruh browser operator yang terbuka tanpa perlu *polling*.
- **Python (FastAPI)** (*Alternatif Unggulan*):
  - Sangat cepat, memiliki dukungan *asynchronous* bawaan (`asyncio`), auto-generasi dokumentasi Swagger UI (`/docs`), serta memudahkan bila ingin mengintegrasikan komparasi fuzzy menggunakan *library* ilmiah seperti `scikit-fuzzy`.

#### 3. Database: PostgreSQL / MySQL
- Menyimpan tabel relasional `users`, `sensor_logs`, dan `hazard_events`.
- Mampu menangani ribuan baris log riwayat sensor per hari dengan pengindeksan waktu (*timestamp indexing*).

#### 4. Frontend Dashboard: HTML5 + Tailwind CSS + Vanilla JS / React
- Menghasilkan antarmuka bersih yang responsif diakses dari ponsel teknisi maupun monitor kontrol ruang operator.
- Menampilkan 4 kartu telemetri utama persis seperti perancangan antarmuka proposal:
  1. **Kartu Gas LPG**: ppm + Status (Aman/Waspada/Bahaya).
  2. **Kartu Konsentrasi Asap**: ppm / % + Status.
  3. **Kartu Kondisi Api**: Tidak Ada Api / Terdeteksi / Api Besar + Status.
  4. **Kartu Suhu Lingkungan**: Nilai °C + Status (Normal/Tinggi).
  5. **Panel Status Terpusat**: Indikator Keputusan Fuzzy Logic Mamdani dan Saklar Uji Buzzer.

---

## 4. PERANCANGAN SISTEM FUZZY LOGIC MAMDANI

Sistem menggunakan Sistem Inferensi Fuzzy (*Fuzzy Inference System* / FIS) metode **Mamdani**, yang terdiri atas 4 tahapan: **Fuzzifikasi**, **Pembentukan Aturan (Rule Base)**, **Inferensi MIN–MAX**, dan **Defuzzifikasi Centroid**.

```
    [ Gas LPG (0-5000 ppm) ] ─────┐
    [ Asap (0-1000 ppm)    ] ─────┼──> [ Fuzzifikasi ] ──> [ Evaluasi 27 Aturan ]
    [ Api (0 - 1.0)        ] ─────┘       (Derajat μ)         (Operator MIN)
                                                                    │
                                                                    ▼
                                                            [ Agregasi MAX ]
                                                                    │
                                                                    ▼
    [ Status: Aman / Waspada / Bahaya ] <─── [ Output Crisp K ] <─── [ Defuzzifikasi ]
    [ Kendali Buzzer & LCD            ]      (0 - 100)               (Centroid/CoG)
```

### 4.1 Variabel Input 1: Deteksi Gas LPG ($G$)
- **Domain**: $[0, 5000]$ ppm
- **Himpunan Linguistik**: Aman, Waspada, Bahaya

#### Fungsi Keanggotaan Deteksi Gas:
1. **$\mu_{\text{Aman}}(G)$** (Trapesium Turun):
   $$\mu_{\text{Aman}}(G) = \begin{cases} 
   1, & G = 0 \\
   \frac{1500 - G}{1500}, & 0 < G < 1500 \\
   0, & G \ge 1500 
   \end{cases}$$

2. **$\mu_{\text{Waspada}}(G)$** (Segitiga):
   $$\mu_{\text{Waspada}}(G) = \begin{cases} 
   0, & G \le 500 \text{ atau } G \ge 2500 \\
   \frac{G - 500}{1500 - 500}, & 500 < G \le 1500 \\
   \frac{2500 - G}{2500 - 1500}, & 1500 < G < 2500 
   \end{cases}$$

3. **$\mu_{\text{Bahaya}}(G)$** (Trapesium Naik):
   $$\mu_{\text{Bahaya}}(G) = \begin{cases} 
   0, & G \le 1500 \\
   \frac{G - 1500}{2500 - 1500}, & 1500 < G \le 2500 \\
   \frac{5000 - G}{5000 - 2500}, & 2500 < G < 5000 \\
   1, & G \ge 5000 
   \end{cases}$$

---

### 4.2 Variabel Input 2: Deteksi Asap ($S$)
- **Domain**: $[0, 1000]$ ppm
- **Himpunan Linguistik**: Normal, Waspada, Bahaya

#### Fungsi Keanggotaan Deteksi Asap:
1. **$\mu_{\text{Normal}}(S)$** (Trapesium Turun):
   $$\mu_{\text{Normal}}(S) = \begin{cases} 
   1, & S = 0 \\
   \frac{300 - S}{300}, & 0 < S < 300 \\
   0, & S \ge 300 
   \end{cases}$$

2. **$\mu_{\text{Waspada}}(S)$** (Segitiga):
   $$\mu_{\text{Waspada}}(S) = \begin{cases} 
   0, & S \le 200 \text{ atau } S \ge 800 \\
   \frac{S - 200}{500 - 200}, & 200 < S \le 500 \\
   \frac{800 - S}{800 - 500}, & 500 < S < 800 
   \end{cases}$$

3. **$\mu_{\text{Bahaya}}(S)$** (Trapesium Naik):
   $$\mu_{\text{Bahaya}}(S) = \begin{cases} 
   0, & S \le 700 \\
   \frac{S - 700}{1000 - 700}, & 700 < S \le 1000 \\
   1, & S \ge 1000 
   \end{cases}$$

---

### 4.3 Variabel Input 3: Deteksi Api ($A$)
- **Domain**: $[0, 1.0]$ (Normalisasi ADC Analog Flame Sensor)
- **Himpunan Linguistik**: Tidak Ada, Terdeteksi, Api Besar

#### Fungsi Keanggotaan Deteksi Api:
1. **$\mu_{\text{TidakAda}}(A)$** (Trapesium Turun):
   $$\mu_{\text{TidakAda}}(A) = \begin{cases} 
   1, & A = 0 \\
   \frac{0.5 - A}{0.5}, & 0 < A < 0.5 \\
   0, & A \ge 0.5 
   \end{cases}$$

2. **$\mu_{\text{Terdeteksi}}(A)$** (Segitiga):
   $$\mu_{\text{Terdeteksi}}(A) = \begin{cases} 
   0, & A \le 0.25 \text{ atau } A \ge 0.75 \\
   \frac{A - 0.25}{0.5 - 0.25}, & 0.25 < A \le 0.5 \\
   \frac{0.75 - A}{0.75 - 0.5}, & 0.5 < A < 0.75 
   \end{cases}$$

3. **$\mu_{\text{Besar}}(A)$** (Trapesium Naik):
   $$\mu_{\text{Besar}}(A) = \begin{cases} 
   0, & A \le 0.5 \\
   \frac{A - 0.5}{1.0 - 0.5}, & 0.5 < A < 1.0 \\
   1, & A = 1.0 
   \end{cases}$$

---

### 4.4 Variabel Output: Kondisi Tingkat Kebocoran Gas ($K$)
- **Domain**: $[0, 100]$
- **Himpunan Linguistik**: Aman, Waspada, Bahaya

#### Fungsi Keanggotaan Output:
1. **$\mu_{\text{Aman}}(K)$**:
   $$\mu_{\text{Aman}}(K) = \begin{cases} 
   1, & K = 0 \\
   \frac{50 - K}{50}, & 0 < K < 50 \\
   0, & K \ge 50 
   \end{cases}$$

2. **$\mu_{\text{Waspada}}(K)$**:
   $$\mu_{\text{Waspada}}(K) = \begin{cases} 
   0, & K \le 25 \text{ atau } K \ge 75 \\
   \frac{K - 25}{50 - 25}, & 25 < K \le 50 \\
   \frac{75 - K}{75 - 50}, & 50 < K < 75 
   \end{cases}$$

3. **$\mu_{\text{Bahaya}}(K)$**:
   $$\mu_{\text{Bahaya}}(K) = \begin{cases} 
   0, & K \le 50 \\
   \frac{K - 50}{100 - 50}, & 50 < K < 100 \\
   1, & K = 100 
   \end{cases}$$

---

### 4.5 Basis Aturan Fuzzy (27 Rule Base Mamdani)

Sesuai tabel 3.3.2 proposal, kombinasi aturan $3 \times 3 \times 3 = 27$ aturan:

| Rule ID | Deteksi Gas ($G$) | Deteksi Asap ($S$) | Deteksi Api ($A$) | Output Kondisi ($K$) |
| :---: | :---: | :---: | :---: | :---: |
| **R1** | Aman | Normal | Tidak Ada | **Aman** |
| **R2** | Aman | Normal | Terdeteksi | **Waspada** |
| **R3** | Aman | Normal | Besar | **Bahaya** |
| **R4** | Aman | Waspada | Tidak Ada | **Waspada** |
| **R5** | Aman | Waspada | Terdeteksi | **Waspada** |
| **R6** | Aman | Waspada | Besar | **Bahaya** |
| **R7** | Aman | Bahaya | Tidak Ada | **Bahaya** |
| **R8** | Aman | Bahaya | Terdeteksi | **Bahaya** |
| **R9** | Aman | Bahaya | Besar | **Bahaya** |
| **R10** | Waspada | Normal | Tidak Ada | **Waspada** |
| **R11** | Waspada | Normal | Terdeteksi | **Waspada** |
| **R12** | Waspada | Normal | Besar | **Bahaya** |
| **R13** | Waspada | Waspada | Tidak Ada | **Waspada** |
| **R14** | Waspada | Waspada | Terdeteksi | **Bahaya** |
| **R15** | Waspada | Waspada | Besar | **Bahaya** |
| **R16** | Waspada | Bahaya | Tidak Ada | **Bahaya** |
| **R17** | Waspada | Bahaya | Terdeteksi | **Bahaya** |
| **R18** | Waspada | Bahaya | Besar | **Bahaya** |
| **R19** | Bahaya | Normal | Tidak Ada | **Bahaya** |
| **R20** | Bahaya | Normal | Terdeteksi | **Bahaya** |
| **R21** | Bahaya | Normal | Besar | **Bahaya** |
| **R22** | Bahaya | Waspada | Tidak Ada | **Bahaya** |
| **R23** | Bahaya | Waspada | Terdeteksi | **Bahaya** |
| **R24** | Bahaya | Waspada | Besar | **Bahaya** |
| **R25** | Bahaya | Bahaya | Tidak Ada | **Bahaya** |
| **R26** | Bahaya | Bahaya | Terdeteksi | **Bahaya** |
| **R27** | Bahaya | Bahaya | Besar | **Bahaya** |

### 4.6 Proses Inferensi & Defuzzifikasi Centroid
1. **Evaluasi Aturan (Operator AND / MIN)**:
   $$\alpha_i = \min\left( \mu_{A_i}(G),\, \mu_{B_i}(S),\, \mu_{C_i}(A) \right) \quad \text{untuk } i = 1, 2, \dots, 27$$
2. **Agregasi Hasil (Operator MAX)**:
   $$\mu_{\text{gabungan}}(z) = \max_{i=1}^{27}\left( \min(\alpha_i,\, \mu_{\text{konsekuen}_i}(z)) \right)$$
3. **Defuzzifikasi Centroid (Center of Gravity - CoG)**:
   $$z^* = \frac{\int z \cdot \mu_{\text{gabungan}}(z) \, dz}{\int \mu_{\text{gabungan}}(z) \, dz} \quad \text{atau pendekatan diskrit: } z^* = \frac{\sum_{j=1}^{N} z_j \cdot \mu(z_j)}{\sum_{j=1}^{N} \mu(z_j)}$$
4. **Keputusan Status Akhir Sistem**:
   - Jika $z^* \le 35$ : Status **AMAN**
   - Jika $35 < z^* < 65$ : Status **WASPADA**
   - Jika $z^* \ge 65$ : Status **BAHAYA**

---

## 5. SPESIFIKASI KEBUTUHAN PERANGKAT KERAS (HARDWARE)

| No | Komponen | Tipe / Model | Antarmuka ESP32 | Spesifikasi & Fungsi Teknis |
| :--- | :--- | :--- | :--- | :--- |
| 1 | **Mikrokontroler** | ESP32 DOIT DevKit V1 | Main Board | Dual-Core 240 MHz, 520 KB SRAM, Wi-Fi 802.11 b/g/n 2.4 GHz, ADC 12-bit (0–4095). |
| 2 | **Sensor Gas LPG** | MQ-2 Gas Sensor Modul | Pin Analog -> GPIO 34 (ADC1_CH6) | Mendeteksi hidrokarbon LPG (propana, butana) 200–5000 ppm. Tegangan kerja 5V DC. |
| 3 | **Sensor Asap** | MQ-2 Sensor Modul | Pin Analog -> GPIO 35 (ADC1_CH7) | Kalibrasi rasio kurva sensitivitas asap ($R_s/R_0$) 100–1000 ppm. |
| 4 | **Sensor Nyala Api** | Flame Sensor (IR Receiver) | Pin Analog -> GPIO 32 (ADC1_CH4)<br>Pin Digital -> GPIO 33 | Deteksi inframerah 760–1100 nm, sudut deteksi 60°, trimpot sensitivitas onboard. |
| 5 | **Sensor Suhu (Lingkungan)** | DHT11 / DHT22 | Pin Digital Data -> GPIO 4 | Membaca temperatur 0–50°C (±2°C) dan kelembapan 20–90% RH (komplementer dashboard). |
| 6 | **Output Visual Lokal** | LCD Display 16x2 + I2C | I2C (SDA -> GPIO 21, SCL -> GPIO 22) | Alamat default I2C `0x27` / `0x3F`. Menampilkan 2 baris teks: Baris 1: Nilai Gas & Asap; Baris 2: Status Fuzzy & Api. |
| 7 | **Output Alarm Lokal** | Active Buzzer 5V | GPIO 19 via Transistor NPN (2N2222) | Menghasilkan nada frekuensi alarm (Waspada: bip intermiten; Bahaya: nada kontinu frekuensi tinggi). |
| 8 | **Regulator & Catu Daya** | Adapter 5V / 2A + LM2596 / AMS1117 | VIN & GND | Pemanas (heater) pada MQ-2 membutuhkan arus stabil ~150-180 mA per sensor. |

---

## 6. KEBUTUHAN FUNGSIONAL SISTEM (FUNCTIONAL REQUIREMENTS)

### 6.1 Subsistem Firmware ESP32 (FR-FW)
- **FR-FW-01 (Sampling & Averaging)**: ESP32 harus membaca data sensor analog (MQ-2 Gas, MQ-2 Asap, Flame Sensor) dengan teknik *multi-sampling* (10 sampel per pembacaan) untuk meredam derau (*noise*) ADC.
- **FR-FW-02 (Perhitungan PPM)**: ESP32 harus mengonversi tegangan analog MQ-2 menjadi nilai ppm gas dan asap menggunakan kurva karakteristik resistansi sensor ($R_s/R_0$) hasil kalibrasi awal di udara bersih.
- **FR-FW-03 (Eksekusi Fuzzy Mamdani)**: ESP32 wajib mengeksekusi perhitungan fungsi keanggotaan, evaluasi 27 aturan, dan defuzzifikasi setiap 1 detik.
- **FR-FW-04 (Pembaruan Display Lokal)**: ESP32 harus memperbarui tampilan LCD 16x2 setiap 1 detik tanpa memblokir proses komputasi (*non-blocking timer via `millis()`*).
- **FR-FW-05 (Kendali Buzzer)**:
  - Kondisi Aman: Buzzer MATI (LOW).
  - Kondisi Waspada: Buzzer berbunyi bip terputus-putus (500 ms ON, 500 ms OFF).
  - Kondisi Bahaya: Buzzer berbunyi secara kontinu (HIGH) tanpa jeda.
- **FR-FW-06 (Koneksi & Telemetri Jaringan)**: ESP32 harus terhubung ke Wi-Fi lokal dan mengirimkan payload telemetri JSON ke server backend via HTTP POST atau WebSocket setiap 2–3 detik, atau seketika (*instant event*) saat status berubah ke Bahaya.
- **FR-FW-07 (Failover & Auto-Reconnect)**: Jika koneksi Wi-Fi terputus, ESP32 tetap harus menjalankan deteksi fuzzy, LCD, dan buzzer secara lokal, sambil mencoba rekoneksi otomatis di latar belakang tanpa me-*restart* mikrokontroler.

### 6.2 Subsistem Backend Server & API (FR-BE)
- **FR-BE-01 (Ingestion API)**: Menyediakan REST endpoint `POST /api/v1/telemetry` untuk menerima data dari ESP32 dengan validasi skema JSON dan API Key.
- **FR-BE-02 (Real-Time Broadcasting)**: Menyediakan server WebSocket / Socket.IO untuk meneruskan data telemetri terbaru ke seluruh klien web secara instan (< 500 ms).
- **FR-BE-03 (Penyimpanan Data Historis)**: Menyimpan riwayat pembacaan sensor dan status fuzzy ke tabel database setiap interval berkala (misal: setiap 5 detik saat normal, dan setiap 1 detik saat kondisi Waspada/Bahaya).
- **FR-BE-04 (Autentikasi Pengguna)**: Menyediakan endpoint `POST /api/v1/auth/login` dan `POST /api/v1/auth/register` dengan enkripsi kata sandi menggunakan `bcrypt` dan otorisasi menggunakan JWT (*JSON Web Token*).
- **FR-BE-05 (Log Kejadian Bahaya)**: Mencatat setiap transisi kondisi ke Waspada atau Bahaya ke dalam tabel `alarm_events` untuk kebutuhan audit keselamatan.
- **FR-BE-06 (Data Retrieval & Pagination)**: Menyediakan endpoint `GET /api/v1/telemetry/history` dengan parameter filter rentang waktu dan pagination untuk ditampilkan dalam grafik dan tabel riwayat.

### 6.3 Subsistem Frontend Web Monitoring (FR-FE)
- **FR-FE-01 (Halaman Login & Registrasi)**: Form autentikasi yang aman untuk membatasi akses monitoring hanya kepada petugas yang berwenang.
- **FR-FE-02 (Dashboard Kartu Real-Time)**:
  - Kartu Gas MQ-2: Nilai numerik ppm, progress bar persentase, dan badge status warna (Hijau: Aman, Kuning: Waspada, Merah: Bahaya).
  - Kartu Asap: Nilai ppm / persentase dan badge kondisi.
  - Kartu Flame Sensor: Teks "Tidak Ada Api" / "Terdeteksi Api" beserta indikator visual api.
  - Kartu Suhu: Nilai temperatur ruangan (°C).
- **FR-FE-03 (Indikator Status Utama Fuzzy)**: Banner utama di puncak dashboard yang menampilkan status sistem hasil inferensi Mamdani: **AMAN**, **WASPADA**, atau **BAHAYA** disertai nilai crisp $K$ (0–100).
- **FR-FE-04 (Grafik Tren Real-Time)**: Grafik garis dinamis (*multi-line time-series*) menampilkan pergerakan nilai Gas, Asap, dan Suhu dalam jendela 5–15 menit terakhir.
- **FR-FE-05 (Alarm Audio Web)**: Pemutar suara peringatan darurat pada peramban web browser jika status sistem berubah menjadi Bahaya, dilengkapi tombol *Mute Alarm* (Ack).
- **FR-FE-06 (Tabel Riwayat Insiden)**: Tabel interaktif berisi riwayat deteksi kebocoran sebelumnya dengan kolom Waktu Kejadian, Nilai Gas Tertinggi, Status Api, dan Durasi Kejadian.

---

## 7. KEBUTUHAN NON-FUNGSIONAL (NON-FUNCTIONAL REQUIREMENTS)

### 7.1 Kinerja & Waktu Respon (Performance)
- **NFR-PERF-01 (Latensi End-to-End)**: Waktu yang dibutuhkan sejak gas/api terdeteksi pada sensor hingga data tampil pada Web Dashboard tidak boleh melebihi **2,0 detik** (sesuai tolok ukur Maulana 2024 & Arif 2024 pada proposal).
- **NFR-PERF-02 (Waktu Respon Lokal)**: Respon aktivasi Buzzer dan pembaruan LCD terhadap perubahan status bahaya harus terjadi dalam waktu **< 200 ms**.
- **NFR-PERF-03 (Throughput Backend)**: Backend server harus mampu menangani minimal 20 request per detik per node tanpa degradasi memori.

### 7.2 Keandalan & Ketersediaan (Reliability & Availability)
- **NFR-REL-01 (Operasi Berkelanjutan)**: Sistem dirancang untuk beroperasi secara terus menerus (24/7). ESP32 dilengkapi perangkat keras *Watchdog Timer (WDT)* dengan batas waktu 5 detik untuk mencegah *system hanging*.
- **NFR-REL-02 (Data Integrity)**: Data telemetri yang gagal dikirim saat Wi-Fi terputus dapat disimpan sementara pada *ring-buffer* memori RAM ESP32 dan dikirim ulang setelah koneksi pulih (*optional store-and-forward*).

### 7.3 Keamanan (Security)
- **NFR-SEC-01 (Autentikasi API)**: Komunikasi pengiriman data dari ESP32 ke backend diamankan dengan header `X-API-KEY`.
- **NFR-SEC-02 (Password Hashing)**: Seluruh kata sandi pengguna web wajib di-hash menggunakan algoritma `bcrypt` dengan *salt rounds* minimal 10.
- **NFR-SEC-03 (Protected Routes)**: Akses ke halaman dashboard dan API data historis wajib memvalidasi token JWT yang valid.

### 7.4 Kemudahan Penggunaan & Kompatibilitas (Usability)
- **NFR-USE-01 (Responsif Antarmuka)**: Dashboard web harus dapat dibuka dengan tata letak optimal pada resolusi ponsel pintar (360x640 px), tablet, hingga layar desktop (1920x1080 px).
- **NFR-USE-02 (Cross-Browser)**: Antarmuka harus berfungsi normal pada peramban modern (Google Chrome, Mozilla Firefox, Microsoft Edge, Safari).

---

## 8. SPESIFIKASI DATA & RANCANGAN API

### 8.1 Skema Database Relasional

```sql
-- Tabel Pengguna Web Monitoring
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) DEFAULT 'operator', -- 'admin', 'operator', 'viewer'
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Tabel Log Telemetri Sensor
CREATE TABLE sensor_telemetry (
    id BIGSERIAL PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL DEFAULT 'ESP32-GAS-01',
    gas_ppm FLOAT NOT NULL,
    smoke_ppm FLOAT NOT NULL,
    flame_raw FLOAT NOT NULL,
    flame_status VARCHAR(20) NOT NULL, -- 'TIDAK_ADA', 'TERDETEKSI', 'BESAR'
    temperature FLOAT NULL,
    humidity FLOAT NULL,
    fuzzy_crisp_output FLOAT NOT NULL, -- 0 - 100
    fuzzy_condition VARCHAR(20) NOT NULL, -- 'AMAN', 'WASPADA', 'BAHAYA'
    buzzer_status BOOLEAN NOT NULL DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX idx_telemetry_created_at ON sensor_telemetry(created_at);

-- Tabel Riwayat Kejadian Insiden / Alarm
CREATE TABLE alarm_events (
    id SERIAL PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    event_type VARCHAR(20) NOT NULL, -- 'WASPADA', 'BAHAYA'
    peak_gas_ppm FLOAT NOT NULL,
    flame_triggered BOOLEAN NOT NULL,
    started_at TIMESTAMP WITH TIME ZONE NOT NULL,
    resolved_at TIMESTAMP WITH TIME ZONE NULL,
    notes TEXT NULL
);
```

### 8.2 Kontrak API Telemetri (ESP32 ke Backend)

#### Endpoint Ingest Telemetri
- **Method**: `POST`
- **URL**: `/api/v1/telemetry`
- **Headers**:
  ```http
  Content-Type: application/json
  X-API-KEY: secret_esp32_gas_token_2026
  ```
- **Request Body (JSON)**:
  ```json
  {
    "deviceId": "ESP32-GAS-01",
    "gasPpm": 617.4,
    "smokePpm": 210.8,
    "flameRaw": 0.05,
    "flameStatus": "TIDAK_ADA",
    "temperature": 29.2,
    "fuzzyCrisp": 48.7,
    "fuzzyStatus": "WASPADA",
    "buzzerActive": true
  }
  ```
- **Response**:
  ```json
  {
    "status": "success",
    "message": "Telemetry received and broadcasted",
    "serverTime": "2026-10-16T12:25:00Z"
  }
  ```

### 8.3 Kontrak WebSocket (Backend ke Web Dashboard)
- **Event Name**: `sensor_update`
- **Payload Event**:
  ```json
  {
    "timestamp": "2026-10-16T12:25:00.120Z",
    "deviceId": "ESP32-GAS-01",
    "data": {
      "gas": { "value": 617.4, "unit": "ppm", "status": "WASPADA" },
      "smoke": { "value": 210.8, "unit": "ppm", "status": "NORMAL" },
      "flame": { "value": 0.05, "status": "TIDAK_ADA", "detected": false },
      "temperature": { "value": 29.2, "unit": "°C", "status": "NORMAL" },
      "system": {
        "score": 48.7,
        "condition": "WASPADA",
        "buzzer": true
      }
    }
  }
  ```

---

## 9. RENCANA PENGUJIAN & KRITERIA KEBERHASILAN (TEST PLAN)

Rencana pengujian dirancang mengacu pada Bab 3.3.3 hingga 3.3.7 proposal Proyek Akhir:

### 9.1 Matriks Pengujian Sensor MQ-2 (Jarak terhadap Sumber Gas)
| No | Jarak Sensor ke Sumber Gas | Kondisi Uji | Parameter yang Dinilai | Kriteria Keberhasilan |
| :---: | :---: | :---: | :---: | :---: |
| 1 | 5 cm | Sangat Dekat | Respon ppm & lonjakan tegangan | Terdeteksi seketika (> 1500 ppm), respon < 1 detik |
| 2 | 10 cm | Dekat | Akurasi pembacaan ppm | Terdeteksi signifikan (800–2500 ppm) |
| 3 | 15 cm | Sedang | Perubahan nilai analog ADC | Terdeteksi (500–1500 ppm) |
| 4 | 20 cm | Jauh | Sensitivitas difusi gas di udara | Terdeteksi (> 300 ppm) |
| 5 | 25 cm | Maksimum | Batas jangkauan deteksi | Nilai sensor mulai meluruh mendekati batas normal |

### 9.2 Matriks Pengujian Flame Sensor (Jarak terhadap Sumber Api)
| No | Jarak Sensor ke Sumber Api | Kondisi Deteksi Sesuai Proposal | Kriteria Keberhasilan |
| :---: | :---: | :---: | :---: |
| 1–5 | 5 cm – 25 cm | Terdeteksi | Nilai Flame > 0.5 (Status: Api Terdeteksi / Besar) |
| 6 | 30 cm | Terdeteksi (bergantung intensitas) | Terdeteksi jika nyala api cukup stabil |
| 7 | 35 cm | Sensitivitas mulai menurun | Nilai analog fluktuatif di sekitar 0.25–0.4 |
| 8 | 40 cm | Sulit terdeteksi | Sinyal sangat rendah |
| 9–10| 45 cm – 50 cm | Tidak terdeteksi | Nilai Flame = 0 (Tidak Ada Api) |

### 9.3 Matriks Pengujian Validasi 4 Skenario Nyata Sistem
| Skenario | Kondisi Riil | Input MQ-2 Gas | Input Flame | Output Fuzzy | Status LCD | Status Buzzer | Status Web |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Skenario 1** | Normal | Rendah (< 500 ppm) | Tidak ada api | **Aman** | Aman | MATI | Aman (Hijau) |
| **Skenario 2** | Gas Rendah | Sedang (500–1500 ppm) | Tidak ada api | **Waspada** | Waspada | AKTIF (Intermiten) | Waspada (Kuning) |
| **Skenario 3** | Gas Tinggi | Tinggi (> 1500 ppm) | Tidak ada api | **Bahaya** | Bahaya | AKTIF (Kontinu) | Bahaya (Merah) |
| **Skenario 4** | Api Muncul | Bebas / Sedang | Ada api | **Bahaya** | Bahaya | AKTIF (Kontinu) | Bahaya (Merah) |

### 9.4 Pengujian Black-Box Web Monitoring
1. **Autentikasi**: Pengujian pendaftaran user baru, login berhasil dengan kredensial valid, dan penolakan akses untuk halaman terlindungi jika token tidak ada.
2. **Koneksi ESP32**: Pengujian penerimaan data telemetri menggunakan Wi-Fi lokal, verifikasi data tersimpan utuh di database.
3. **Pembaruan Real-Time**: Verifikasi waktu dari perubahan sensor di alat fisik hingga perubahan angka di kartu web via stopwatch (< 2 detik).
4. **Stabilitas Monitoring**: Pengujian *stress test* monitoring berkelanjutan selama 60 menit tanpa *disconnect* atau *crash*.

---

## 10. ROADMAP IMPLEMENTASI & DELIVERABLES

```
Minggu 1: Persiapan & Firmware Edge
├── Pengadaan & Kalibrasi Sensor (MQ-2, Flame Sensor, LCD, Buzzer)
├── Wiring ESP32 & Validasi ADC Multi-Sampling
└── Pemrograman Algoritma Fuzzy Logic Mamdani 27 Rules (C++) di ESP32

Minggu 2: Pengembangan Backend & Database
├── Inisialisasi Project Backend (Node.js/Express atau Python/FastAPI)
├── Konfigurasi Skema Database (Users, Telemetry, Alarm Events)
├── Pembuatan REST API Ingestion & Autentikasi JWT
└── Implementasi WebSocket / Socket.IO Server

Minggu 3: Pengembangan Frontend Web Dashboard
├── Desain Layout Responsif Tailwind CSS (Mockup 4 Kartu Sensor)
├── Integrasi Socket.IO Client untuk Pembaruan Angka & Status Real-time
├── Pembuatan Grafik Tren Waktu Riil (Chart.js)
└── Implementasi Web Audio Peringatan Suara Bahaya

Minggu 4: Pengujian Terpadu, Kalibrasi, & Dokumentasi
├── Pengujian Skenario Jarak Sensor (5–25 cm MQ-2, 5–50 cm Flame)
├── Pengujian 4 Skenario Nyata & Pencatatan Latensi Transmisi
├── Uji Black-box Fungsionalitas Lengkap
└── Penyusunan Laporan Proyek Akhir (Bab 4 & 5)
```

---
*Dokumen ini disusun sebagai cetak biru teknis resmi untuk merealisasikan Proyek Akhir Sarjana Terapan PJJ Teknik Telekomunikasi PENS 2026.*
