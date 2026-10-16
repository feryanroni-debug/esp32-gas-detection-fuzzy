// app.js - Logika Frontend Real-Time & Web Audio Alert

let audioCtx = null;
let alarmOscillator = null;
let isMuted = false;

// Inisialisasi Audio Context (Web Audio API)
function playAlarmTone() {
    if (isMuted) return;
    try {
        if (!audioCtx) {
            audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        }
        if (audioCtx.state === 'suspended') {
            audioCtx.resume();
        }

        if (!alarmOscillator) {
            const osc = audioCtx.createOscillator();
            const gain = audioCtx.createGain();
            
            osc.type = 'sawtooth';
            osc.frequency.setValueAtTime(850, audioCtx.currentTime); // 850 Hz alarm
            gain.gain.setValueAtTime(0.15, audioCtx.currentTime);
            
            osc.connect(gain);
            gain.connect(audioCtx.destination);
            osc.start();
            alarmOscillator = { osc, gain };
        }
    } catch (e) {
        console.warn('Audio belum diizinkan oleh peramban:', e);
    }
}

function stopAlarmTone() {
    if (alarmOscillator) {
        try {
            alarmOscillator.osc.stop();
            alarmOscillator.osc.disconnect();
        } catch (e) {}
        alarmOscillator = null;
    }
}

document.addEventListener('DOMContentLoaded', () => {
    // 1. Inisialisasi Chart
    if (typeof window.initTelemetryChart === 'function') {
        window.initTelemetryChart();
    }

    // 2. Tombol Mute / Ack Alarm
    const muteBtn = document.getElementById('muteBtn');
    if (muteBtn) {
        muteBtn.addEventListener('click', () => {
            isMuted = !isMuted;
            if (isMuted) {
                stopAlarmTone();
                muteBtn.classList.remove('bg-gray-700');
                muteBtn.classList.add('bg-amber-600');
                muteBtn.innerText = '🔇 Alarm Di-Mute';
            } else {
                muteBtn.classList.remove('bg-amber-600');
                muteBtn.classList.add('bg-gray-700');
                muteBtn.innerText = '🔊 Audio Aktif';
            }
        });
    }

    // 3. Tombol Uji Buzzer / Alarm
    const testBuzzerBtn = document.getElementById('testBuzzerBtn');
    if (testBuzzerBtn && window.socket) {
        testBuzzerBtn.addEventListener('click', () => {
            window.socket.emit('test_buzzer', { source: 'Web Dashboard' });
            alert('Sinyal uji alarm dikirim ke server.');
        });
    }

    // 4. Inisialisasi Koneksi Socket.IO
    initSocketConnection();
});

function initSocketConnection() {
    const socket = io();
    window.socket = socket;

    const connectionBadge = document.getElementById('connectionBadge');

    socket.on('connect', () => {
        if (connectionBadge) {
            connectionBadge.className = 'px-3 py-1 text-xs font-semibold rounded-full bg-emerald-500/20 text-emerald-400 border border-emerald-500/30';
            connectionBadge.innerText = '● Terhubung (Live)';
        }
    });

    socket.on('disconnect', () => {
        if (connectionBadge) {
            connectionBadge.className = 'px-3 py-1 text-xs font-semibold rounded-full bg-rose-500/20 text-rose-400 border border-rose-500/30';
            connectionBadge.innerText = '○ Terputus';
        }
        stopAlarmTone();
    });

    // Menerima data sensor berkala dari backend
    socket.on('sensor_update', (payload) => {
        updateDashboardUI(payload);
    });
}

function updateDashboardUI(payload) {
    const { timestamp, data } = payload;
    const { gas, smoke, flame, temperature, system } = data;

    // 1. Update Nilai & Tampilan Kartu Gas LPG
    const gasValEl = document.getElementById('gasValue');
    const gasBadgeEl = document.getElementById('gasBadge');
    const gasProgressEl = document.getElementById('gasProgress');
    if (gasValEl) gasValEl.innerText = gas.value.toFixed(1);
    if (gasBadgeEl) {
        gasBadgeEl.innerText = gas.status;
        gasBadgeEl.className = getStatusBadgeClass(gas.status);
    }
    if (gasProgressEl) {
        const gasPercent = Math.min(100, (gas.value / 3000) * 100);
        gasProgressEl.style.width = `${gasPercent}%`;
    }

    // 2. Update Kartu Asap
    const smokeValEl = document.getElementById('smokeValue');
    const smokeBadgeEl = document.getElementById('smokeBadge');
    if (smokeValEl) smokeValEl.innerText = smoke.value.toFixed(1);
    if (smokeBadgeEl) {
        smokeBadgeEl.innerText = smoke.status;
        smokeBadgeEl.className = getStatusBadgeClass(smoke.status);
    }

    // 3. Update Kartu Api
    const flameValEl = document.getElementById('flameValue');
    const flameBadgeEl = document.getElementById('flameBadge');
    if (flameValEl) {
        flameValEl.innerText = flame.detected ? '🔥 API TERDETEKSI' : 'AMAN';
        flameValEl.className = flame.detected ? 'text-2xl font-bold text-rose-500' : 'text-2xl font-bold text-emerald-400';
    }
    if (flameBadgeEl) {
        flameBadgeEl.innerText = flame.status;
        flameBadgeEl.className = flame.detected ? 'px-2 py-0.5 rounded text-xs font-medium bg-rose-500/20 text-rose-400' : 'px-2 py-0.5 rounded text-xs font-medium bg-emerald-500/20 text-emerald-400';
    }

    // 4. Update Kartu Suhu
    const tempValEl = document.getElementById('tempValue');
    if (tempValEl) tempValEl.innerText = temperature.value.toFixed(1);

    // 5. Update Banner Status Utama Fuzzy
    const fuzzyBanner = document.getElementById('fuzzyBanner');
    const fuzzyStatusText = document.getElementById('fuzzyStatusText');
    const fuzzyScoreText = document.getElementById('fuzzyScoreText');
    const lastUpdateText = document.getElementById('lastUpdateText');

    if (fuzzyStatusText) fuzzyStatusText.innerText = system.condition;
    if (fuzzyScoreText) fuzzyScoreText.innerText = `Skor Crisp: ${system.score.toFixed(1)} / 100`;
    if (lastUpdateText) lastUpdateText.innerText = `Update: ${new Date(timestamp).toLocaleTimeString()}`;

    if (fuzzyBanner) {
        fuzzyBanner.className = 'p-6 rounded-2xl border transition-all duration-300 ' + getFuzzyBannerClass(system.condition);
    }

    // 6. Kendali Audio Alarm Web
    if (system.condition === 'BAHAYA') {
        playAlarmTone();
    } else {
        stopAlarmTone();
    }

    // 7. Update Grafik Time-Series
    if (typeof window.updateChartData === 'function') {
        window.updateChartData(timestamp, gas.value, smoke.value, temperature.value);
    }
}

function getStatusBadgeClass(status) {
    if (status === 'BAHAYA') {
        return 'px-2.5 py-1 rounded-md text-xs font-semibold bg-rose-500/20 text-rose-400 border border-rose-500/30';
    } else if (status === 'WASPADA') {
        return 'px-2.5 py-1 rounded-md text-xs font-semibold bg-amber-500/20 text-amber-400 border border-amber-500/30';
    }
    return 'px-2.5 py-1 rounded-md text-xs font-semibold bg-emerald-500/20 text-emerald-400 border border-emerald-500/30';
}

function getFuzzyBannerClass(condition) {
    if (condition === 'BAHAYA') {
        return 'bg-rose-950/40 border-rose-500 text-rose-200 status-danger-pulse';
    } else if (condition === 'WASPADA') {
        return 'bg-amber-950/40 border-amber-500 text-amber-200 status-warning-pulse';
    }
    return 'bg-emerald-950/30 border-emerald-500/50 text-emerald-200';
}
