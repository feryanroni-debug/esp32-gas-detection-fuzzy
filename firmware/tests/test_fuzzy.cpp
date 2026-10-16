#include <iostream>
#include <iomanip>
#include <vector>
#include "../esp32_gas_detection/FuzzyEngine.h"

// Implementasi dummy fungsi jika dijalankan di compiler desktop
int main() {
    FuzzyEngine engine;

    struct TestCase {
        std::string name;
        float gasPpm;
        float smokePpm;
        float flameVal;
        std::string expectedStatus;
    };

    std::vector<TestCase> testCases = {
        // Skenario 1: Normal (Udara Bersih)
        {"Skenario 1.1: Udara Sangat Bersih", 50.0f, 20.0f, 0.0f, "AMAN"},
        {"Skenario 1.2: Normal Menengah", 250.0f, 80.0f, 0.05f, "AMAN"},
        {"Skenario 1.3: Normal Ambang Atas", 450.0f, 150.0f, 0.1f, "AMAN"},

        // Skenario 2: Gas Rendah / Waspada Awal
        {"Skenario 2.1: Gas Mulai Bocor", 800.0f, 250.0f, 0.0f, "WASPADA"},
        {"Skenario 2.2: Gas Sedang & Asap Ringan", 1200.0f, 350.0f, 0.15f, "WASPADA"},
        {"Skenario 2.3: Asap Waspada Tinggi", 400.0f, 600.0f, 0.0f, "WASPADA"},

        // Skenario 3: Gas Tinggi / Kritis (> 1500 ppm)
        {"Skenario 3.1: Gas Tinggi (> 1500 ppm)", 2200.0f, 300.0f, 0.0f, "BAHAYA"},
        {"Skenario 3.2: Kebocoran Gas Masif", 3000.0f, 500.0f, 0.0f, "BAHAYA"},
        {"Skenario 3.3: Gas Maksimum", 4800.0f, 800.0f, 0.0f, "BAHAYA"},

        // Skenario 4: Nyala Api Muncul
        {"Skenario 4.1: Api Kecil Terdeteksi (Gas Rendah)", 200.0f, 50.0f, 0.45f, "WASPADA"},
        {"Skenario 4.2: Api Signifikan Muncul", 400.0f, 100.0f, 0.85f, "BAHAYA"},
        {"Skenario 4.3: Gas Bocor Disertai Api", 1500.0f, 400.0f, 0.70f, "BAHAYA"}
    };

    std::cout << "========================================================================================\n";
    std::cout << "               PENGUJIAN VALIDASI ALGORITMA FUZZY LOGIC MAMDANI (27 RULES)              \n";
    std::cout << "========================================================================================\n";
    std::cout << std::left 
              << std::setw(38) << "Kasus Uji" 
              << std::setw(10) << "Gas(ppm)" 
              << std::setw(10) << "Asap(ppm)" 
              << std::setw(10) << "Api(0-1)" 
              << std::setw(12) << "Skor (K)" 
              << std::setw(10) << "Status" 
              << std::setw(8)  << "Hasil" 
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    int passed = 0;
    for (const auto& tc : testCases) {
        FuzzyResult res = engine.compute(tc.gasPpm, tc.smokePpm, tc.flameVal);
        bool isPass = (res.conditionStr == tc.expectedStatus);
        if (isPass) passed++;

        std::cout << std::left 
                  << std::setw(38) << tc.name
                  << std::setw(10) << std::fixed << std::setprecision(1) << tc.gasPpm
                  << std::setw(10) << std::fixed << std::setprecision(1) << tc.smokePpm
                  << std::setw(10) << std::fixed << std::setprecision(2) << tc.flameVal
                  << std::setw(12) << std::fixed << std::setprecision(2) << res.crispScore
                  << std::setw(10) << res.conditionStr
                  << std::setw(8)  << (isPass ? "[PASS]" : "[FAIL]")
                  << "\n";
    }

    std::cout << "========================================================================================\n";
    std::cout << "Total Pengujian: " << testCases.size() << " | Berhasil: " << passed 
              << " | Gagal: " << (testCases.size() - passed) << "\n";
    std::cout << "========================================================================================\n";

    return (passed == (int)testCases.size()) ? 0 : 1;
}
