-- ===================================================
-- SKEMA DATABASE: SISTEM MONITORING KEBOCORAN GAS
-- ===================================================

-- 1. Tabel Pengguna Web Monitoring
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) DEFAULT 'operator', -- 'admin', 'operator', 'viewer'
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. Tabel Log Telemetri Sensor
CREATE TABLE IF NOT EXISTS sensor_telemetry (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(50) NOT NULL DEFAULT 'ESP32-GAS-01',
    gas_ppm REAL NOT NULL,
    smoke_ppm REAL NOT NULL,
    flame_raw REAL NOT NULL,
    flame_status VARCHAR(20) NOT NULL, -- 'TIDAK_ADA', 'TERDETEKSI', 'BESAR'
    temperature REAL DEFAULT 25.0,
    humidity REAL DEFAULT 60.0,
    fuzzy_crisp_output REAL NOT NULL, -- 0 - 100
    fuzzy_condition VARCHAR(20) NOT NULL, -- 'AMAN', 'WASPADA', 'BAHAYA'
    buzzer_status INTEGER NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index untuk percepatan query time-series
CREATE INDEX IF NOT EXISTS idx_telemetry_created_at ON sensor_telemetry(created_at);

-- 3. Tabel Riwayat Kejadian Insiden / Alarm
CREATE TABLE IF NOT EXISTS alarm_events (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(50) NOT NULL,
    event_type VARCHAR(20) NOT NULL, -- 'WASPADA', 'BAHAYA'
    peak_gas_ppm REAL NOT NULL,
    flame_triggered INTEGER NOT NULL DEFAULT 0,
    started_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    resolved_at TIMESTAMP,
    notes TEXT
);
