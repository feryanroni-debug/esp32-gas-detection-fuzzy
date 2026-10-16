<!DOCTYPE html>
<html lang="id" class="dark">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Riwayat Insiden & Log Telemetri - Laravel 13 MySQL</title>
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
</head>
<body class="bg-darkbg text-slate-100 min-h-screen flex flex-col font-sans antialiased">

    <!-- Header -->
    <header class="border-b border-slate-800 bg-cardbg px-6 py-4">
        <div class="max-w-7xl mx-auto flex items-center justify-between">
            <div class="flex items-center space-x-3">
                <span class="text-2xl">📋</span>
                <div>
                    <h1 class="text-lg font-bold text-white">Riwayat Insiden & Log Telemetri</h1>
                    <p class="text-xs text-slate-400">Laravel 13 • MySQL Database Logging</p>
                </div>
            </div>
            <div>
                <a href="{{ route('dashboard') }}" class="px-3 py-1.5 text-xs font-medium rounded-lg bg-slate-700 hover:bg-slate-600 text-white transition">
                    ← Kembali ke Dashboard
                </a>
            </div>
        </div>
    </header>

    <main class="max-w-7xl mx-auto px-6 py-8 flex-1 w-full space-y-6">

        <!-- Tabel Log Alarm & Insiden -->
        <section class="bg-cardbg rounded-2xl p-6 border border-slate-800 shadow-sm">
            <h2 class="text-base font-semibold text-white mb-4">Daftar Kejadian Peringatan & Bahaya (Alarm Events)</h2>
            <div class="overflow-x-auto">
                <table class="w-full text-left text-xs">
                    <thead class="text-slate-400 uppercase bg-slate-900/50 border-b border-slate-800">
                        <tr>
                            <th class="px-4 py-3">Waktu Kejadian</th>
                            <th class="px-4 py-3">Device ID</th>
                            <th class="px-4 py-3">Tingkat Insiden</th>
                            <th class="px-4 py-3">Puncak Gas (ppm)</th>
                            <th class="px-4 py-3">Status Api</th>
                            <th class="px-4 py-3">Keterangan</th>
                        </tr>
                    </thead>
                    <tbody class="divide-y divide-slate-800">
                        @forelse($alarms as $alarm)
                            <tr class="hover:bg-slate-800/40">
                                <td class="px-4 py-3 font-mono">{{ $alarm->started_at->format('d/m/Y H:i:s') }}</td>
                                <td class="px-4 py-3">{{ $alarm->device_id }}</td>
                                <td class="px-4 py-3 font-semibold {{ $alarm->event_type === 'BAHAYA' ? 'text-rose-400' : 'text-amber-400' }}">{{ $alarm->event_type }}</td>
                                <td class="px-4 py-3">{{ number_format($alarm->peak_gas_ppm, 1) }}</td>
                                <td class="px-4 py-3">{{ $alarm->flame_triggered ? '🔥 TERDETEKSI' : 'TIDAK' }}</td>
                                <td class="px-4 py-3 text-slate-400">{{ $alarm->notes ?? '-' }}</td>
                            </tr>
                        @empty
                            <tr>
                                <td colspan="6" class="px-4 py-4 text-center text-slate-500">Belum ada catatan insiden bahaya.</td>
                            </tr>
                        @endforelse
                    </tbody>
                </table>
            </div>
        </section>

        <!-- Tabel Log Telemetri Terakhir -->
        <section class="bg-cardbg rounded-2xl p-6 border border-slate-800 shadow-sm">
            <h2 class="text-base font-semibold text-white mb-4">Log Telemetri Sensor Terkini</h2>
            <div class="overflow-x-auto">
                <table class="w-full text-left text-xs">
                    <thead class="text-slate-400 uppercase bg-slate-900/50 border-b border-slate-800">
                        <tr>
                            <th class="px-4 py-3">Waktu</th>
                            <th class="px-4 py-3">Gas (ppm)</th>
                            <th class="px-4 py-3">Asap (ppm)</th>
                            <th class="px-4 py-3">Api (Raw)</th>
                            <th class="px-4 py-3">Suhu</th>
                            <th class="px-4 py-3">Fuzzy Crisp</th>
                            <th class="px-4 py-3">Status Sistem</th>
                        </tr>
                    </thead>
                    <tbody class="divide-y divide-slate-800">
                        @forelse($telemetries as $tel)
                            <tr class="hover:bg-slate-800/40">
                                <td class="px-4 py-2 font-mono">{{ $tel->created_at->format('H:i:s') }}</td>
                                <td class="px-4 py-2 font-semibold text-orange-400">{{ number_format($tel->gas_ppm, 1) }}</td>
                                <td class="px-4 py-2">{{ number_format($tel->smoke_ppm, 1) }}</td>
                                <td class="px-4 py-2">{{ number_format($tel->flame_raw, 2) }} ({{ $tel->flame_status }})</td>
                                <td class="px-4 py-2 text-cyan-400">{{ $tel->temperature ? number_format($tel->temperature, 1).'°C' : '-' }}</td>
                                <td class="px-4 py-2">{{ number_format($tel->fuzzy_crisp_output, 1) }}</td>
                                <td class="px-4 py-2 font-bold {{ $tel->fuzzy_condition === 'BAHAYA' ? 'text-rose-400' : ($tel->fuzzy_condition === 'WASPADA' ? 'text-amber-400' : 'text-emerald-400') }}">
                                    {{ $tel->fuzzy_condition }}
                                </td>
                            </tr>
                        @empty
                            <tr>
                                <td colspan="7" class="px-4 py-4 text-center text-slate-500">Belum ada riwayat telemetri.</td>
                            </tr>
                        @endforelse
                    </tbody>
                </table>
            </div>
        </section>

    </main>

    <footer class="border-t border-slate-800 px-6 py-4 text-center text-xs text-slate-500">
        Laravel 13 Architecture • MySQL Database • PENS 2026
    </footer>
</body>
</html>
