const db = require('../config/database');

exports.saveTelemetry = (req, res) => {
    const {
        deviceId = 'ESP32-GAS-01',
        gasPpm,
        smokePpm,
        flameRaw,
        flameStatus = 'TIDAK_ADA',
        temperature = 28.0,
        humidity = 60.0,
        fuzzyCrisp,
        fuzzyStatus,
        buzzerActive = false
    } = req.body;

    if (gasPpm === undefined || smokePpm === undefined || flameRaw === undefined || fuzzyCrisp === undefined || !fuzzyStatus) {
        return res.status(400).json({
            status: 'error',
            message: 'Data telemetri tidak lengkap. Mohon periksa format payload.'
        });
    }

    const sql = `
        INSERT INTO sensor_telemetry 
        (device_id, gas_ppm, smoke_ppm, flame_raw, flame_status, temperature, humidity, fuzzy_crisp_output, fuzzy_condition, buzzer_status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    `;
    const params = [
        deviceId,
        gasPpm,
        smokePpm,
        flameRaw,
        flameStatus,
        temperature,
        humidity,
        fuzzyCrisp,
        fuzzyStatus,
        buzzerActive ? 1 : 0
    ];

    db.run(sql, params, function (err) {
        if (err) {
            console.error('❌ Error menyimpan telemetri:', err.message);
            return res.status(500).json({ status: 'error', message: 'Gagal menyimpan ke database' });
        }

        const telemetryId = this.lastID;
        const nowIso = new Date().toISOString();

        // Format payload sesuai kontrak PRD 8.3
        const socketPayload = {
            id: telemetryId,
            timestamp: nowIso,
            deviceId: deviceId,
            data: {
                gas: { value: gasPpm, unit: 'ppm', status: gasPpm > 1500 ? 'BAHAYA' : (gasPpm > 500 ? 'WASPADA' : 'AMAN') },
                smoke: { value: smokePpm, unit: 'ppm', status: smokePpm > 700 ? 'BAHAYA' : (smokePpm > 200 ? 'WASPADA' : 'NORMAL') },
                flame: { value: flameRaw, status: flameStatus, detected: flameRaw >= 0.25 },
                temperature: { value: temperature, unit: '°C', status: temperature > 35 ? 'TINGGI' : 'NORMAL' },
                system: {
                    score: fuzzyCrisp,
                    condition: fuzzyStatus,
                    buzzer: buzzerActive
                }
            }
        };

        // Siarkan secara instan ke WebSocket klien
        const io = req.app.get('socketio');
        if (io) {
            io.emit('sensor_update', socketPayload);
        }

        // Catat ke tabel alarm_events jika kondisi Waspada / Bahaya
        if (fuzzyStatus === 'WASPADA' || fuzzyStatus === 'BAHAYA') {
            const alarmSql = `
                INSERT INTO alarm_events (device_id, event_type, peak_gas_ppm, flame_triggered, notes)
                VALUES (?, ?, ?, ?, ?)
            `;
            db.run(alarmSql, [
                deviceId,
                fuzzyStatus,
                gasPpm,
                flameStatus !== 'TIDAK_ADA' ? 1 : 0,
                `Peringatan ${fuzzyStatus} terdeteksi oleh inferensi Fuzzy Logic (Score: ${fuzzyCrisp})`
            ]);
        }

        return res.status(201).json({
            status: 'success',
            message: 'Telemetry received and broadcasted',
            telemetryId: telemetryId,
            serverTime: nowIso
        });
    });
};

exports.getLatestTelemetry = (req, res) => {
    const sql = `SELECT * FROM sensor_telemetry ORDER BY created_at DESC LIMIT 1`;
    db.get(sql, [], (err, row) => {
        if (err) {
            return res.status(500).json({ status: 'error', message: err.message });
        }
        return res.json({ status: 'success', data: row || null });
    });
};

exports.getTelemetryHistory = (req, res) => {
    const limit = parseInt(req.query.limit) || 50;
    const sql = `SELECT * FROM sensor_telemetry ORDER BY created_at DESC LIMIT ?`;
    db.all(sql, [limit], (err, rows) => {
        if (err) {
            return res.status(500).json({ status: 'error', message: err.message });
        }
        return res.json({ status: 'success', data: rows.reverse() });
    });
};

exports.getAlarmEvents = (req, res) => {
    const limit = parseInt(req.query.limit) || 20;
    const sql = `SELECT * FROM alarm_events ORDER BY started_at DESC LIMIT ?`;
    db.all(sql, [limit], (err, rows) => {
        if (err) {
            return res.status(500).json({ status: 'error', message: err.message });
        }
        return res.json({ status: 'success', data: rows });
    });
};
