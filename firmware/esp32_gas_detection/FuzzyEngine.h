#ifndef FUZZY_ENGINE_H
#define FUZZY_ENGINE_H

#if defined(ARDUINO) || defined(ESP32)
    #include <Arduino.h>
#else
    #include <algorithm>
    #include <cmath>
    #include <string>
    #include <iostream>
    #include <vector>
    using std::min;
    using std::max;
#endif

// Enum Status Kondisi Sistem
enum SystemCondition {
    CONDITION_AMAN = 0,
    CONDITION_WASPADA = 1,
    CONDITION_BAHAYA = 2
};

// Struktur Hasil Inferensi Fuzzy
struct FuzzyResult {
    float crispScore;          // Nilai tegas z* (0.0 - 100.0)
    SystemCondition condition; // Status: CONDITION_AMAN, CONDITION_WASPADA, CONDITION_BAHAYA
    const char* conditionStr;  // "AMAN", "WASPADA", "BAHAYA"
    float alphaAman;           // Nilai agregasi alfa untuk konsekuen Aman
    float alphaWaspada;        // Nilai agregasi alfa untuk konsekuen Waspada
    float alphaBahaya;         // Nilai agregasi alfa untuk konsekuen Bahaya
};

class FuzzyEngine {
public:
    FuzzyEngine();

    /**
     * @brief Menjalankan inferensi Fuzzy Logic Mamdani secara menyeluruh
     * @param gasPpm Konsentrasi gas LPG (0 - 5000 ppm)
     * @param smokePpm Konsentrasi asap (0 - 1000 ppm)
     * @param flameVal Nilai sensor api yang dinormalisasi (0.0 - 1.0)
     * @return FuzzyResult Berisi skor crisp (0-100), status, dan nilai alfa
     */
    FuzzyResult compute(float gasPpm, float smokePpm, float flameVal);

    // ==========================================
    // FUNGSI KEANGGOTAAN INPUT: GAS LPG (G) [0 - 5000 ppm]
    // ==========================================
    static float muGasAman(float g);
    static float muGasWaspada(float g);
    static float muGasBahaya(float g);

    // ==========================================
    // FUNGSI KEANGGOTAAN INPUT: ASAP (S) [0 - 1000 ppm]
    // ==========================================
    static float muAsapNormal(float s);
    static float muAsapWaspada(float s);
    static float muAsapBahaya(float s);

    // ==========================================
    // FUNGSI KEANGGOTAAN INPUT: API (A) [0.0 - 1.0]
    // ==========================================
    static float muApiTidakAda(float a);
    static float muApiTerdeteksi(float a);
    static float muApiBesar(float a);

    // ==========================================
    // FUNGSI KEANGGOTAAN OUTPUT: KONDISI KEBOCORAN (K) [0 - 100]
    // ==========================================
    static float muOutputAman(float k);
    static float muOutputWaspada(float k);
    static float muOutputBahaya(float k);

private:
    float evaluateRulesAndDefuzzify(
        float uG_Aman, float uG_Waspada, float uG_Bahaya,
        float uS_Normal, float uS_Waspada, float uS_Bahaya,
        float uA_TidakAda, float uA_Terdeteksi, float uA_Besar,
        float &outAlphaAman, float &outAlphaWaspada, float &outAlphaBahaya
    );
};

#endif // FUZZY_ENGINE_H
