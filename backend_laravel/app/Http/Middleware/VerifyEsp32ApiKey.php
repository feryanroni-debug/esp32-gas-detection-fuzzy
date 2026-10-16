<?php

namespace App\Http\Middleware;

use Closure;
use Illuminate\Http\Request;
use Symfony\Component\HttpFoundation\Response;

class VerifyEsp32ApiKey
{
    /**
     * Handle an incoming request.
     */
    public function handle(Request $request, Closure $next): Response
    {
        $apiKey = $request->header('X-API-KEY');
        $validKey = config('services.esp32.api_key', env('ESP32_API_KEY', 'secret_esp32_gas_token_2026'));

        if (!$apiKey || $apiKey !== $validKey) {
            return response()->json([
                'status' => 'error',
                'message' => 'Akses ditolak: X-API-KEY tidak valid atau tidak disertakan.'
            ], 401);
        }

        return $next($request);
    }
}
