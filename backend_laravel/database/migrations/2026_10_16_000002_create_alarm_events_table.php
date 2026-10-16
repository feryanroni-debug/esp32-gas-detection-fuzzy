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
        Schema::create('alarm_events', function (Blueprint $table) {
            $table->id();
            $table->string('device_id', 50)->default('ESP32-GAS-01');
            $table->string('event_type', 20); // WASPADA, BAHAYA
            $table->float('peak_gas_ppm');
            $table->boolean('flame_triggered')->default(false);
            $table->timestamp('started_at')->useCurrent();
            $table->timestamp('resolved_at')->nullable();
            $table->text('notes')->nullable();
            $table->timestamps();
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::dropIfExists('alarm_events');
    }
};
