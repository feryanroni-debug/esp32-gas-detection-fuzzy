<?php

use App\Http\Controllers\TelemetryController;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes - Sistem Deteksi Kebocoran Gas ESP32
|--------------------------------------------------------------------------
*/

// Endpoint Ingest Telemetri dari ESP32 (Wajib Header X-API-KEY)
Route::middleware('esp32.key')->group(function () {
    Route::post('/v1/telemetry', [TelemetryController::class, 'store']);
});

// Endpoint Pengambilan Data Publik / Web Dashboard Polling
Route::prefix('v1/telemetry')->group(function () {
    Route::get('/latest', [TelemetryController::class, 'latest']);
    Route::get('/history', [TelemetryController::class, 'history']);
    Route::get('/alarms', [TelemetryController::class, 'alarms']);
});

// Health check
Route::get('/health', function () {
    return response()->json([
        'status' => 'ok',
        'framework' => 'Laravel 13',
        'database' => config('database.default'),
        'timestamp' => now()->toISOString(),
    ]);
});
