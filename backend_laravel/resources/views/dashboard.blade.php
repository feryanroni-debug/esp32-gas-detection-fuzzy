<!DOCTYPE html>
<html lang="id" class="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Laravel 13 - Monitoring Deteksi Gas & Api (ESP32 Fuzzy Logic)</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <script>
        tailwind.config = {
            darkMode: 'class',
            theme: {
                extend: {
                    colors: {
                        darkbg: '#0f172a',
                        cardbg: '#1e293b'
                    }
                }
            }
        }
    </script>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        @keyframes pulse-danger {
            0%, 100% { box-shadow: 0 0 0 0 rgba(239, 68, 68, 0.7); }
            50% { box-shadow: 0 0 20px 10px rgba(239, 68, 68, 0.4); }
        }
        @keyframes pulse-warning {
            0%, 100% { box-shadow: 0 0 0 0 rgba(245, 158, 11, 0.6); }
            50% { box-shadow: 0 0 15px 5px rgba(245, 158, 11, 0.3); }
        }
        .status-danger-pulse { animation: pulse-danger 1.2s infinite; }
        .status-warning-pulse { animation: pulse-warning 2s infinite; }
    </style>
</head>
<body class="bg-darkbg text-slate-100 min-h-screen flex flex-col font-sans antialiased">

    <!-- Header -->
    <header class="border-b border-slate-800 bg-cardbg/80 backdrop-blur sticky top-0 z-50 px-6 py-4">
        <div class="max-w-7xl mx-auto flex items-center justify-between">
            <div class="flex items-center space-x-3">
                <span class="text-2xl">🔥</span>
                <div>
                    <h1 class="text-lg font-bold tracking-tight text-white">Sistem Deteksi Gas, Asap & Api</h1>
                    <p class="text-xs text-slate-400">Laravel 13 • MySQL • ESP32 Fuzzy Logic Mamdani 27 Rules</p>
                </div>
            </div>

            <div class="flex items-center space-x-4">
                <span id="connectionBadge" class="px-3 py-1 text-xs font-semibold rounded-full bg-emerald-500/20 text-emerald-400 border border-emerald-500/30">
                    ● Real-Time Polling
                </span>
                <button id="muteBtn" class="px-3 py-1.5 text-xs font-medium rounded-lg bg-gray-700 hover:bg-gray-600 text-white transition">
                    🔊 Audio Aktif
                </button>
                <a href="{{ route('history') }}" class="px-3 py-1.5 text-xs font-medium rounded-lg bg-slate-700 hover:bg-slate-600 text-white transition">
                    📋 Riwayat Insiden
                </a>
            </div>
        </div>
    </header>

    <!-- Konten Utama -->
    <main class="max-w-7xl mx-auto px-6 py-8 flex-1 w-full space-y-6">

        <!-- Banner Status Fuzzy Mamdani -->
        <section id="fuzzyBanner" class="p-6 rounded-2xl border border-emerald-500/40 bg-emerald-950/20 text-emerald-200 transition-all duration-300">
            <div class="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4">
                <div>
                    <span class="text-xs font-bold uppercase tracking-wider text-slate-400">Status Keputusan Sistem (Fuzzy Logic Mamdani)</span>
                    <div class="flex items-baseline space-x-3 mt-1">
                        <h2 id="fuzzyStatusText" class="text-4xl font-extrabold tracking-tight">{{ $latest ? $latest->fuzzy_condition : 'AMAN' }}</h2>
                        <span id="fuzzyScoreText" class="text-sm font-medium text-slate-300">Skor Crisp: {{ $latest ? number_format($latest->fuzzy_crisp_output, 1) : '0.0' }} / 100</span>
                    </div>
                </div>
                <div class="text-right flex flex-col sm:items-end">
                    <span id="lastUpdateText" class="text-xs text-slate-400 mb-2">Update: {{ $latest ? $latest->created_at->format('H:i:s') : 'Menunggu data...' }}</span>
                </div>
            </div>
        </section>

        <!-- Grid 4 Kartu Sensor -->
        <section class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-6">

            <!-- 1. Gas LPG -->
            <div class="bg-cardbg rounded-2xl p-5 border border-slate-800 hover:border-slate-700 transition shadow-sm">
                <div class="flex items-center justify-between text-slate-400 mb-3">
                    <span class="text-xs font-semibold uppercase tracking-wider">Gas LPG (MQ-2)</span>
                    <span id="gasBadge" class="px-2 py-0.5 rounded text-xs font-semibold bg-emerald-500/20 text-emerald-400">AMAN</span>
                </div>
                <div class="flex items-baseline space-x-2">
                    <span id="gasValue" class="text-3xl font-extrabold text-white">{{ $latest ? number_format($latest->gas_ppm, 1) : '0.0' }}</span>
                    <span class="text-sm text-slate-400">ppm</span>
                </div>
                <div class="w-full bg-slate-800 rounded-full h-1.5 mt-4 overflow-hidden">
                    <div id="gasProgress" class="bg-orange-500 h-1.5 rounded-full transition-all duration-300" style="width: {{ $latest ? min(100, ($latest->gas_ppm / 3000) * 100) : 0 }}%"></div>
                </div>
            </div>

            <!-- 2. Asap -->
            <div class="bg-cardbg rounded-2xl p-5 border border-slate-800 hover:border-slate-700 transition shadow-sm">
                <div class="flex items-center justify-between text-slate-400 mb-3">
                    <span class="text-xs font-semibold uppercase tracking-wider">Asap (MQ-2)</span>
                    <span id="smokeBadge" class="px-2 py-0.5 rounded text-xs font-semibold bg-emerald-500/20 text-emerald-400">NORMAL</span>
                </div>
                <div class="flex items-baseline space-x-2">
                    <span id="smokeValue" class="text-3xl font-extrabold text-white">{{ $latest ? number_format($latest->smoke_ppm, 1) : '0.0' }}</span>
                    <span class="text-sm text-slate-400">ppm</span>
                </div>
                <p class="text-xs text-slate-500 mt-4">Ambang batas waspada: &gt; 200 ppm</p>
            </div>

            <!-- 3. Api -->
            <div class="bg-cardbg rounded-2xl p-5 border border-slate-800 hover:border-slate-700 transition shadow-sm">
                <div class="flex items-center justify-between text-slate-400 mb-3">
                    <span class="text-xs font-semibold uppercase tracking-wider">Deteksi Api</span>
                    <span id="flameBadge" class="px-2 py-0.5 rounded text-xs font-semibold bg-emerald-500/20 text-emerald-400">{{ $latest ? $latest->flame_status : 'TIDAK ADA' }}</span>
                </div>
                <div class="flex items-baseline space-x-2">
                    <span id="flameValue" class="text-2xl font-bold {{ ($latest && $latest->flame_raw >= 0.25) ? 'text-rose-500' : 'text-emerald-400' }}">
                        {{ ($latest && $latest->flame_raw >= 0.25) ? '🔥 API TERDETEKSI' : 'AMAN' }}
                    </span>
                </div>
                <p class="text-xs text-slate-500 mt-4">Sensor Inframerah Spektrum 760–1100 nm</p>
            </div>

            <!-- 4. Suhu -->
            <div class="bg-cardbg rounded-2xl p-5 border border-slate-800 hover:border-slate-700 transition shadow-sm">
                <div class="flex items-center justify-between text-slate-400 mb-3">
                    <span class="text-xs font-semibold uppercase tracking-wider">Suhu Ruangan</span>
                    <span class="px-2 py-0.5 rounded text-xs font-semibold bg-blue-500/20 text-blue-400">NORMAL</span>
                </div>
                <div class="flex items-baseline space-x-2">
                    <span id="tempValue" class="text-3xl font-extrabold text-white">{{ $latest && $latest->temperature ? number_format($latest->temperature, 1) : '28.0' }}</span>
                    <span class="text-sm text-slate-400">°C</span>
                </div>
                <p class="text-xs text-slate-500 mt-4">Parameter Lingkungan Komplementer</p>
            </div>

        </section>

        <!-- Grafik Real-Time -->
        <section class="bg-cardbg rounded-2xl p-6 border border-slate-800 shadow-sm">
            <div class="flex items-center justify-between mb-4">
                <div>
                    <h3 class="text-base font-semibold text-white">Grafik Tren Waktu Nyata</h3>
                    <p class="text-xs text-slate-400">Pergerakan konsentrasi Gas, Asap, dan Suhu</p>
                </div>
                <div class="flex items-center space-x-2 text-xs text-slate-400">
                    <span class="inline-block w-3 h-3 rounded-full bg-orange-500"></span> <span>Gas</span>
                    <span class="inline-block w-3 h-3 rounded-full bg-slate-400 ml-2"></span> <span>Asap</span>
                    <span class="inline-block w-3 h-3 rounded-full bg-cyan-400 ml-2"></span> <span>Suhu</span>
                </div>
            </div>
            <div class="relative w-full h-72 sm:h-80">
                <canvas id="telemetryChart"></canvas>
            </div>
        </section>

    </main>

    <footer class="border-t border-slate-800 px-6 py-4 text-center text-xs text-slate-500">
        Laravel 13 Architecture • MySQL Database • ESP32 Fuzzy Logic • PENS 2026
    </footer>

    <script>
        let sensorChart = null;
        let isMuted = false;
        let audioCtx = null;
        let alarmOsc = null;

        function playAlarm() {
            if (isMuted) return;
            try {
                if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
                if (audioCtx.state === 'suspended') audioCtx.resume();
                if (!alarmOsc) {
                    const osc = audioCtx.createOscillator();
                    const gain = audioCtx.createGain();
                    osc.type = 'sawtooth';
                    osc.frequency.setValueAtTime(850, audioCtx.currentTime);
                    gain.gain.setValueAtTime(0.15, audioCtx.currentTime);
                    osc.connect(gain);
                    gain.connect(audioCtx.destination);
                    osc.start();
                    alarmOsc = { osc, gain };
                }
            } catch (e) {}
        }

        function stopAlarm() {
            if (alarmOsc) {
                try { alarmOsc.osc.stop(); alarmOsc.osc.disconnect(); } catch (e) {}
                alarmOsc = null;
            }
        }

        document.getElementById('muteBtn').addEventListener('click', function() {
            isMuted = !isMuted;
            if (isMuted) {
                stopAlarm();
                this.classList.replace('bg-gray-700', 'bg-amber-600');
                this.innerText = '🔇 Alarm Di-Mute';
            } else {
                this.classList.replace('bg-amber-600', 'bg-gray-700');
                this.innerText = '🔊 Audio Aktif';
            }
        });

        // Inisialisasi Chart
        function initChart() {
            const ctx = document.getElementById('telemetryChart');
            sensorChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        { label: 'Gas LPG (ppm)', data: [], borderColor: '#f97316', backgroundColor: 'rgba(249, 115, 22, 0.1)', borderWidth: 2, tension: 0.3, fill: true },
                        { label: 'Asap (ppm)', data: [], borderColor: '#64748b', borderWidth: 2, tension: 0.3, fill: false },
                        { label: 'Suhu (°C)', data: [], borderColor: '#06b6d4', borderDash: [5, 5], borderWidth: 2, fill: false }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        x: { grid: { color: 'rgba(255, 255, 255, 0.05)' }, ticks: { color: '#94a3b8' } },
                        y: { grid: { color: 'rgba(255, 255, 255, 0.05)' }, ticks: { color: '#94a3b8' } }
                    },
                    plugins: { legend: { labels: { color: '#e2e8f0' } } }
                }
            });
        }

        async function fetchLatest() {
            try {
                const res = await fetch('/api/v1/telemetry/latest');
                const json = await res.json();
                if (json.data) {
                    const d = json.data;
                    document.getElementById('gasValue').innerText = Number(d.gas_ppm).toFixed(1);
                    document.getElementById('smokeValue').innerText = Number(d.smoke_ppm).toFixed(1);
                    document.getElementById('tempValue').innerText = Number(d.temperature || 28).toFixed(1);
                    
                    const isFire = Number(d.flame_raw) >= 0.25;
                    const flameVal = document.getElementById('flameValue');
                    flameVal.innerText = isFire ? '🔥 API TERDETEKSI' : 'AMAN';
                    flameVal.className = isFire ? 'text-2xl font-bold text-rose-500' : 'text-2xl font-bold text-emerald-400';
                    document.getElementById('flameBadge').innerText = d.flame_status;

                    document.getElementById('fuzzyStatusText').innerText = d.fuzzy_condition;
                    document.getElementById('fuzzyScoreText').innerText = `Skor Crisp: ${Number(d.fuzzy_crisp_output).toFixed(1)} / 100`;
                    document.getElementById('lastUpdateText').innerText = `Update: ${new Date(d.created_at).toLocaleTimeString()}`;

                    const banner = document.getElementById('fuzzyBanner');
                    if (d.fuzzy_condition === 'BAHAYA') {
                        banner.className = 'p-6 rounded-2xl border transition-all duration-300 bg-rose-950/40 border-rose-500 text-rose-200 status-danger-pulse';
                        playAlarm();
                    } else if (d.fuzzy_condition === 'WASPADA') {
                        banner.className = 'p-6 rounded-2xl border transition-all duration-300 bg-amber-950/40 border-amber-500 text-amber-200 status-warning-pulse';
                        stopAlarm();
                    } else {
                        banner.className = 'p-6 rounded-2xl border transition-all duration-300 bg-emerald-950/20 border-emerald-500/40 text-emerald-200';
                        stopAlarm();
                    }

                    // Update Chart
                    if (sensorChart) {
                        const time = new Date(d.created_at).toLocaleTimeString();
                        if (sensorChart.data.labels.length === 0 || sensorChart.data.labels[sensorChart.data.labels.length - 1] !== time) {
                            sensorChart.data.labels.push(time);
                            sensorChart.data.datasets[0].data.push(d.gas_ppm);
                            sensorChart.data.datasets[1].data.push(d.smoke_ppm);
                            sensorChart.data.datasets[2].data.push(d.temperature || 28);
                            if (sensorChart.data.labels.length > 20) {
                                sensorChart.data.labels.shift();
                                sensorChart.data.datasets.forEach(ds => ds.data.shift());
                            }
                            sensorChart.update();
                        }
                    }
                }
            } catch (e) {}
        }

        initChart();
        setInterval(fetchLatest, 2000);
        fetchLatest();
    </script>
</body>
</html>
