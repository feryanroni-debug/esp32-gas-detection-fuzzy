<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        Schema::create('sensor_telemetries', function (Blueprint $table) {
            $table->id();
            $table->string('device_id', 50)->default('ESP32-GAS-01');
            $table->float('gas_ppm');
            $table->float('smoke_ppm');
            $table->float('flame_raw');
            $table->string('flame_status', 20)->default('TIDAK_ADA');
            $table->float('temperature')->nullable()->default(28.0);
            $table->float('humidity')->nullable()->default(60.0);
            $table->float('fuzzy_crisp_output'); // 0 - 100
            $table->string('fuzzy_condition', 20); // AMAN, WASPADA, BAHAYA
            $table->boolean('buzzer_status')->default(false);
            $table->timestamps();

            // Indexing kolom created_at untuk performa query time-series
            $table->index('created_at');
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::dropIfExists('sensor_telemetries');
    }
};
