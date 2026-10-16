<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class SensorTelemetry extends Model
{
    use HasFactory;

    protected $table = 'sensor_telemetries';

    protected $fillable = [
        'device_id',
        'gas_ppm',
        'smoke_ppm',
        'flame_raw',
        'flame_status',
        'temperature',
        'humidity',
        'fuzzy_crisp_output',
        'fuzzy_condition',
        'buzzer_status',
    ];

    protected $casts = [
        'gas_ppm' => 'float',
        'smoke_ppm' => 'float',
        'flame_raw' => 'float',
        'temperature' => 'float',
        'humidity' => 'float',
        'fuzzy_crisp_output' => 'float',
        'buzzer_status' => 'boolean',
    ];
}
