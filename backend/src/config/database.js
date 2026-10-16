const fs = require('fs');
const path = require('path');

const dbDir = path.resolve(__dirname, '../../database');
if (!fs.existsSync(dbDir)) {
    fs.mkdirSync(dbDir, { recursive: true });
}

const dbFilePath = path.join(dbDir, 'gas_detection_db.json');

// Struktur default database
let dbData = {
    users: [],
    sensor_telemetry: [],
    alarm_events: [],
    counters: {
        users: 1,
        sensor_telemetry: 1,
        alarm_events: 1
    }
};

// Muat data dari file jika ada
if (fs.existsSync(dbFilePath)) {
    try {
        const fileContent = fs.readFileSync(dbFilePath, 'utf8');
        dbData = JSON.parse(fileContent);
        console.log('✅ Berhasil memuat database lokal dari:', dbFilePath);
    } catch (e) {
        console.warn('⚠️ Gagal membaca berkas database yang ada, membuat database baru.');
    }
} else {
    saveDatabaseSync();
    console.log('✅ Database lokal baru berhasil diinisialisasi:', dbFilePath);
}

function saveDatabase() {
    try {
        fs.writeFileSync(dbFilePath, JSON.stringify(dbData, null, 2), 'utf8');
    } catch (err) {
        console.error('❌ Gagal menyimpan database ke disk:', err.message);
    }
}

function saveDatabaseSync() {
    try {
        fs.writeFileSync(dbFilePath, JSON.stringify(dbData, null, 2), 'utf8');
    } catch (err) {}
}

/**
 * Driver Database Kompatibel SQLite
 * Mendukung antarmuka db.run, db.get, db.all, dan db.exec
 */
const db = {
    run: function (sql, params, callback) {
        if (typeof params === 'function') {
            callback = params;
            params = [];
        }
        params = params || [];
        callback = callback || function () {};

        const sqlTrim = sql.trim().toUpperCase();

        try {
            if (sqlTrim.startsWith('INSERT INTO USERS')) {
                const [name, email, password_hash, role] = params;
                // Cek keunikan email
                const existing = dbData.users.find(u => u.email.toLowerCase() === email.toLowerCase());
                if (existing) {
                    return callback.call({ lastID: null }, new Error('UNIQUE constraint failed: users.email'));
                }
                const newId = dbData.counters.users++;
                const newUser = {
                    id: newId,
                    name,
                    email: email.toLowerCase(),
                    password_hash,
                    role: role || 'operator',
                    created_at: new Date().toISOString()
                };
                dbData.users.push(newUser);
                saveDatabase();
                return callback.call({ lastID: newId }, null);
            }

            if (sqlTrim.startsWith('INSERT INTO SENSOR_TELEMETRY')) {
                const [
                    device_id, gas_ppm, smoke_ppm, flame_raw, flame_status,
                    temperature, humidity, fuzzy_crisp_output, fuzzy_condition, buzzer_status
                ] = params;

                const newId = dbData.counters.sensor_telemetry++;
                const newTelemetry = {
                    id: newId,
                    device_id,
                    gas_ppm: Number(gas_ppm),
                    smoke_ppm: Number(smoke_ppm),
                    flame_raw: Number(flame_raw),
                    flame_status,
                    temperature: Number(temperature),
                    humidity: Number(humidity),
                    fuzzy_crisp_output: Number(fuzzy_crisp_output),
                    fuzzy_condition,
                    buzzer_status: buzzer_status ? 1 : 0,
                    created_at: new Date().toISOString()
                };
                dbData.sensor_telemetry.push(newTelemetry);
                saveDatabase();
                return callback.call({ lastID: newId }, null);
            }

            if (sqlTrim.startsWith('INSERT INTO ALARM_EVENTS')) {
                const [device_id, event_type, peak_gas_ppm, flame_triggered, notes] = params;
                const newId = dbData.counters.alarm_events++;
                const newAlarm = {
                    id: newId,
                    device_id,
                    event_type,
                    peak_gas_ppm: Number(peak_gas_ppm),
                    flame_triggered: flame_triggered ? 1 : 0,
                    started_at: new Date().toISOString(),
                    resolved_at: null,
                    notes: notes || ''
                };
                dbData.alarm_events.push(newAlarm);
                saveDatabase();
                return callback.call({ lastID: newId }, null);
            }

            return callback.call({ lastID: 1 }, null);
        } catch (err) {
            return callback(err);
        }
    },

    get: function (sql, params, callback) {
        if (typeof params === 'function') {
            callback = params;
            params = [];
        }
        params = params || [];
        callback = callback || function () {};

        const sqlTrim = sql.trim().toUpperCase();

        try {
            if (sqlTrim.includes('FROM USERS WHERE EMAIL = ?')) {
                const email = (params[0] || '').toLowerCase();
                const user = dbData.users.find(u => u.email === email);
                return callback(null, user ? { ...user } : null);
            }

            if (sqlTrim.includes('FROM SENSOR_TELEMETRY ORDER BY CREATED_AT DESC LIMIT 1')) {
                const len = dbData.sensor_telemetry.length;
                const item = len > 0 ? dbData.sensor_telemetry[len - 1] : null;
                return callback(null, item ? { ...item } : null);
            }

            return callback(null, null);
        } catch (err) {
            return callback(err);
        }
    },

    all: function (sql, params, callback) {
        if (typeof params === 'function') {
            callback = params;
            params = [];
        }
        params = params || [];
        callback = callback || function () {};

        const sqlTrim = sql.trim().toUpperCase();

        try {
            if (sqlTrim.includes('FROM SENSOR_TELEMETRY')) {
                const limit = Number(params[0]) || 50;
                // Urutkan created_at descending (terbaru lebih dahulu)
                const sorted = [...dbData.sensor_telemetry].reverse().slice(0, limit);
                return callback(null, sorted);
            }

            if (sqlTrim.includes('FROM ALARM_EVENTS')) {
                const limit = Number(params[0]) || 20;
                const sorted = [...dbData.alarm_events].reverse().slice(0, limit);
                return callback(null, sorted);
            }

            return callback(null, []);
        } catch (err) {
            return callback(err);
        }
    },

    exec: function (sql, callback) {
        callback = callback || function () {};
        return callback(null);
    }
};

module.exports = db;
