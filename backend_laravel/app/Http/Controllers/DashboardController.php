<?php

namespace App\Http\Controllers;

use App\Models\AlarmEvent;
use App\Models\SensorTelemetry;
use Illuminate\View\View;

class DashboardController extends Controller
{
    /**
     * Menampilkan halaman Dashboard Monitoring Real-Time
     */
    public function index(): View
    {
        $latest = SensorTelemetry::latest()->first();

        return view('dashboard', [
            'latest' => $latest,
        ]);
    }

    /**
     * Menampilkan halaman Riwayat Insiden & Log Telemetri
     */
    public function history(): View
    {
        $alarms = AlarmEvent::latest()->take(30)->get();
        $telemetries = SensorTelemetry::latest()->take(30)->get();

        return view('history', [
            'alarms' => $alarms,
            'telemetries' => $telemetries,
        ]);
    }
}
