<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class AlarmEvent extends Model
{
    use HasFactory;

    protected $table = 'alarm_events';

    protected $fillable = [
        'device_id',
        'event_type',
        'peak_gas_ppm',
        'flame_triggered',
        'started_at',
        'resolved_at',
        'notes',
    ];

    protected $casts = [
        'peak_gas_ppm' => 'float',
        'flame_triggered' => 'boolean',
        'started_at' => 'datetime',
        'resolved_at' => 'datetime',
    ];
}
