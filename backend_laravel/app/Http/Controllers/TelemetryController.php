<?php

namespace App\Http\Controllers;

use App\Models\AlarmEvent;
use App\Models\SensorTelemetry;
use Illuminate\Http\Request;
use Illuminate\Http\JsonResponse;

class TelemetryController extends Controller
{
    /**
     * Menyimpan data telemetri yang dikirimkan oleh ESP32
     */
    public function store(Request $request): JsonResponse
    {
        $validated = $request->validate([
            'deviceId' => 'nullable|string|max:50',
            'gasPpm' => 'required|numeric',
            'smokePpm' => 'required|numeric',
            'flameRaw' => 'required|numeric',
            'flameStatus' => 'nullable|string|max:20',
            'temperature' => 'nullable|numeric',
            'humidity' => 'nullable|numeric',
            'fuzzyCrisp' => 'required|numeric',
            'fuzzyStatus' => 'required|string|max:20',
            'buzzerActive' => 'nullable|boolean',
        ]);

        $deviceId = $validated['deviceId'] ?? 'ESP32-GAS-01';
        $fuzzyStatus = strtoupper($validated['fuzzyStatus']);

        // 1. Simpan ke tabel sensor_telemetries
        $telemetry = SensorTelemetry::create([
            'device_id' => $deviceId,
            'gas_ppm' => $validated['gasPpm'],
            'smoke_ppm' => $validated['smokePpm'],
            'flame_raw' => $validated['flameRaw'],
            'flame_status' => $validated['flameStatus'] ?? 'TIDAK_ADA',
            'temperature' => $validated['temperature'] ?? 28.0,
            'humidity' => $validated['humidity'] ?? 60.0,
            'fuzzy_crisp_output' => $validated['fuzzyCrisp'],
            'fuzzy_condition' => $fuzzyStatus,
            'buzzer_status' => $validated['buzzerActive'] ?? false,
        ]);

        // 2. Jika kondisi Waspada atau Bahaya, catat ke tabel alarm_events
        if (in_array($fuzzyStatus, ['WASPADA', 'BAHAYA'])) {
            AlarmEvent::create([
                'device_id' => $deviceId,
                'event_type' => $fuzzyStatus,
                'peak_gas_ppm' => $validated['gasPpm'],
                'flame_triggered' => ($validated['flameStatus'] ?? 'TIDAK_ADA') !== 'TIDAK_ADA',
                'started_at' => now(),
                'notes' => "Peringatan {$fuzzyStatus} terdeteksi oleh inferensi Fuzzy Logic (Skor: {$validated['fuzzyCrisp']})",
            ]);
        }

        return response()->json([
            'status' => 'success',
            'message' => 'Telemetry received and saved successfully',
            'telemetryId' => $telemetry->id,
            'serverTime' => now()->toISOString(),
        ], 201);
    }

    /**
     * Mengambil data telemetri terkini
     */
    public function latest(): JsonResponse
    {
        $latest = SensorTelemetry::latest()->first();

        return response()->json([
            'status' => 'success',
            'data' => $latest,
        ]);
    }

    /**
     * Mengambil riwayat telemetri untuk grafik Chart.js
     */
    public function history(Request $request): JsonResponse
    {
        $limit = (int) $request->query('limit', 25);
        $records = SensorTelemetry::latest()->take($limit)->get()->reverse()->values();

        return response()->json([
            'status' => 'success',
            'data' => $records,
        ]);
    }

    /**
     * Mengambil daftar log alarm dan insiden
     */
    public function alarms(Request $request): JsonResponse
    {
        $limit = (int) $request->query('limit', 20);
        $alarms = AlarmEvent::latest()->take($limit)->get();

        return response()->json([
            'status' => 'success',
            'data' => $alarms,
        ]);
    }
}
