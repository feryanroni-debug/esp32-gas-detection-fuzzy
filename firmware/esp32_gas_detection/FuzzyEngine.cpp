#include "FuzzyEngine.h"

FuzzyEngine::FuzzyEngine() {}

// =========================================================================
// 1. FUNGSI KEANGGOTAAN: GAS LPG (G) [0 - 5000 ppm]
// =========================================================================
float FuzzyEngine::muGasAman(float g) {
    if (g <= 0.0f) return 1.0f;
    if (g < 1500.0f) return (1500.0f - g) / 1500.0f;
    return 0.0f;
}

float FuzzyEngine::muGasWaspada(float g) {
    if (g <= 500.0f || g >= 2500.0f) return 0.0f;
    if (g > 500.0f && g <= 1500.0f) return (g - 500.0f) / 1000.0f;
    return (2500.0f - g) / 1000.0f;
}

float FuzzyEngine::muGasBahaya(float g) {
    if (g <= 1500.0f) return 0.0f;
    if (g < 2500.0f) return (g - 1500.0f) / 1000.0f;
    return 1.0f; // G >= 2500 ppm bernilai 1.0 (Trapesium Naik)
}

// =========================================================================
// 2. FUNGSI KEANGGOTAAN: ASAP (S) [0 - 1000 ppm]
// =========================================================================
float FuzzyEngine::muAsapNormal(float s) {
    if (s <= 0.0f) return 1.0f;
    if (s < 300.0f) return (300.0f - s) / 300.0f;
    return 0.0f;
}

float FuzzyEngine::muAsapWaspada(float s) {
    if (s <= 200.0f || s >= 800.0f) return 0.0f;
    if (s > 200.0f && s <= 500.0f) return (s - 200.0f) / 300.0f;
    return (800.0f - s) / 300.0f;
}

float FuzzyEngine::muAsapBahaya(float s) {
    if (s <= 700.0f) return 0.0f;
    if (s < 1000.0f) return (s - 700.0f) / 300.0f;
    return 1.0f; // S >= 1000 ppm bernilai 1.0 (Trapesium Naik)
}

// =========================================================================
// 3. FUNGSI KEANGGOTAAN: API (A) [0.0 - 1.0]
// =========================================================================
float FuzzyEngine::muApiTidakAda(float a) {
    if (a <= 0.0f) return 1.0f;
    if (a < 0.5f) return (0.5f - a) / 0.5f;
    return 0.0f;
}

float FuzzyEngine::muApiTerdeteksi(float a) {
    if (a <= 0.25f || a >= 0.75f) return 0.0f;
    if (a > 0.25f && a <= 0.5f) return (a - 0.25f) / 0.25f;
    return (0.75f - a) / 0.25f;
}

float FuzzyEngine::muApiBesar(float a) {
    if (a <= 0.5f) return 0.0f;
    if (a < 1.0f) return (a - 0.5f) / 0.5f;
    return 1.0f; // A >= 1.0 bernilai 1.0 (Trapesium Naik)
}

// =========================================================================
// 4. FUNGSI KEANGGOTAAN OUTPUT: TINGKAT KONDISI KEBOCORAN (K) [0 - 100]
// =========================================================================
float FuzzyEngine::muOutputAman(float k) {
    if (k <= 0.0f) return 1.0f;
    if (k < 50.0f) return (50.0f - k) / 50.0f;
    return 0.0f;
}

float FuzzyEngine::muOutputWaspada(float k) {
    if (k <= 25.0f || k >= 75.0f) return 0.0f;
    if (k > 25.0f && k <= 50.0f) return (k - 25.0f) / 25.0f;
    return (75.0f - k) / 25.0f;
}

float FuzzyEngine::muOutputBahaya(float k) {
    if (k <= 50.0f) return 0.0f;
    if (k < 100.0f) return (k - 50.0f) / 50.0f;
    return 1.0f;
}

// =========================================================================
// 5. EVALUASI 27 ATURAN MAMDANI & DEFUZZIFIKASI CENTROID (COG)
// =========================================================================
float FuzzyEngine::evaluateRulesAndDefuzzify(
    float uG_Aman, float uG_Waspada, float uG_Bahaya,
    float uS_Normal, float uS_Waspada, float uS_Bahaya,
    float uA_TidakAda, float uA_Terdeteksi, float uA_Besar,
    float &outAlphaAman, float &outAlphaWaspada, float &outAlphaBahaya
) {
    float alpha_Aman = 0.0f;
    float alpha_Waspada = 0.0f;
    float alpha_Bahaya = 0.0f;

    auto min3 = [](float a, float b, float c) -> float {
        return min(a, min(b, c));
    };

    // -------------------------------------------------------------
    // GRUP 1: Gas = AMAN
    // -------------------------------------------------------------
    // R1: IF Gas Aman AND Asap Normal AND Api Tidak Ada THEN Kondisi AMAN
    alpha_Aman = max(alpha_Aman, min3(uG_Aman, uS_Normal, uA_TidakAda));
    // R2: IF Gas Aman AND Asap Normal AND Api Terdeteksi THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Aman, uS_Normal, uA_Terdeteksi));
    // R3: IF Gas Aman AND Asap Normal AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Aman, uS_Normal, uA_Besar));

    // R4: IF Gas Aman AND Asap Waspada AND Api Tidak Ada THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Aman, uS_Waspada, uA_TidakAda));
    // R5: IF Gas Aman AND Asap Waspada AND Api Terdeteksi THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Aman, uS_Waspada, uA_Terdeteksi));
    // R6: IF Gas Aman AND Asap Waspada AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Aman, uS_Waspada, uA_Besar));

    // R7: IF Gas Aman AND Asap Bahaya AND Api Tidak Ada THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Aman, uS_Bahaya, uA_TidakAda));
    // R8: IF Gas Aman AND Asap Bahaya AND Api Terdeteksi THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Aman, uS_Bahaya, uA_Terdeteksi));
    // R9: IF Gas Aman AND Asap Bahaya AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Aman, uS_Bahaya, uA_Besar));

    // -------------------------------------------------------------
    // GRUP 2: Gas = WASPADA
    // -------------------------------------------------------------
    // R10: IF Gas Waspada AND Asap Normal AND Api Tidak Ada THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Waspada, uS_Normal, uA_TidakAda));
    // R11: IF Gas Waspada AND Asap Normal AND Api Terdeteksi THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Waspada, uS_Normal, uA_Terdeteksi));
    // R12: IF Gas Waspada AND Asap Normal AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Normal, uA_Besar));

    // R13: IF Gas Waspada AND Asap Waspada AND Api Tidak Ada THEN Kondisi WASPADA
    alpha_Waspada = max(alpha_Waspada, min3(uG_Waspada, uS_Waspada, uA_TidakAda));
    // R14: IF Gas Waspada AND Asap Waspada AND Api Terdeteksi THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Waspada, uA_Terdeteksi));
    // R15: IF Gas Waspada AND Asap Waspada AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Waspada, uA_Besar));

    // R16: IF Gas Waspada AND Asap Bahaya AND Api Tidak Ada THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Bahaya, uA_TidakAda));
    // R17: IF Gas Waspada AND Asap Bahaya AND Api Terdeteksi THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Bahaya, uA_Terdeteksi));
    // R18: IF Gas Waspada AND Asap Bahaya AND Api Besar THEN Kondisi BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Waspada, uS_Bahaya, uA_Besar));

    // -------------------------------------------------------------
    // GRUP 3: Gas = BAHAYA (Seluruh aturan menghasilkan output BAHAYA)
    // -------------------------------------------------------------
    // R19: Gas Bahaya, Asap Normal, Api Tidak Ada -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Normal, uA_TidakAda));
    // R20: Gas Bahaya, Asap Normal, Api Terdeteksi -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Normal, uA_Terdeteksi));
    // R21: Gas Bahaya, Asap Normal, Api Besar -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Normal, uA_Besar));

    // R22: Gas Bahaya, Asap Waspada, Api Tidak Ada -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Waspada, uA_TidakAda));
    // R23: Gas Bahaya, Asap Waspada, Api Terdeteksi -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Waspada, uA_Terdeteksi));
    // R24: Gas Bahaya, Asap Waspada, Api Besar -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Waspada, uA_Besar));

    // R25: Gas Bahaya, Asap Bahaya, Api Tidak Ada -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Bahaya, uA_TidakAda));
    // R26: Gas Bahaya, Asap Bahaya, Api Terdeteksi -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Bahaya, uA_Terdeteksi));
    // R27: Gas Bahaya, Asap Bahaya, Api Besar -> BAHAYA
    alpha_Bahaya = max(alpha_Bahaya, min3(uG_Bahaya, uS_Bahaya, uA_Besar));

    // Simpan nilai alfa ke variabel output referensi
    outAlphaAman = alpha_Aman;
    outAlphaWaspada = alpha_Waspada;
    outAlphaBahaya = alpha_Bahaya;

    // -------------------------------------------------------------
    // DEFUZZIFIKASI DISKRIT CENTROID (Center of Gravity - CoG)
    // -------------------------------------------------------------
    // Mengintegralkan dari z = 0 hingga z = 100 dengan resolusi langkah dz = 1
    float sumNumerator = 0.0f;
    float sumDenominator = 0.0f;

    for (int z = 0; z <= 100; z++) {
        float fz = (float)z;
        // Implikasi Mamdani (MIN) untuk masing-masing himpunan konsekuen
        float muA = min(alpha_Aman, muOutputAman(fz));
        float muW = min(alpha_Waspada, muOutputWaspada(fz));
        float muB = min(alpha_Bahaya, muOutputBahaya(fz));

        // Agregasi MAX gabungan
        float muGabungan = max(muA, max(muW, muB));

        sumNumerator += (fz * muGabungan);
        sumDenominator += muGabungan;
    }

    if (sumDenominator <= 0.00001f) {
        return 0.0f;
    }

    return sumNumerator / sumDenominator;
}

FuzzyResult FuzzyEngine::compute(float gasPpm, float smokePpm, float flameVal) {
    // 1. Fuzzifikasi Variabel Input
    float uG_Aman = muGasAman(gasPpm);
    float uG_Waspada = muGasWaspada(gasPpm);
    float uG_Bahaya = muGasBahaya(gasPpm);

    float uS_Normal = muAsapNormal(smokePpm);
    float uS_Waspada = muAsapWaspada(smokePpm);
    float uS_Bahaya = muAsapBahaya(smokePpm);

    float uA_TidakAda = muApiTidakAda(flameVal);
    float uA_Terdeteksi = muApiTerdeteksi(flameVal);
    float uA_Besar = muApiBesar(flameVal);

    // 2. Evaluasi 27 Aturan & Defuzzifikasi Centroid
    float alphaAman = 0.0f, alphaWaspada = 0.0f, alphaBahaya = 0.0f;
    float crisp = evaluateRulesAndDefuzzify(
        uG_Aman, uG_Waspada, uG_Bahaya,
        uS_Normal, uS_Waspada, uS_Bahaya,
        uA_TidakAda, uA_Terdeteksi, uA_Besar,
        alphaAman, alphaWaspada, alphaBahaya
    );

    // 3. Pemetaan Keputusan Status Sistem (PRD Section 4.6):
    //    z* <= 35.0         => AMAN
    //    35.0 < z* < 65.0   => WASPADA
    //    z* >= 65.0         => BAHAYA
    FuzzyResult res;
    res.crispScore = crisp;
    res.alphaAman = alphaAman;
    res.alphaWaspada = alphaWaspada;
    res.alphaBahaya = alphaBahaya;

    if (crisp <= 35.0f) {
        res.condition = CONDITION_AMAN;
        res.conditionStr = "AMAN";
    } else if (crisp < 65.0f) {
        res.condition = CONDITION_WASPADA;
        res.conditionStr = "WASPADA";
    } else {
        res.condition = CONDITION_BAHAYA;
        res.conditionStr = "BAHAYA";
    }

    return res;
}
