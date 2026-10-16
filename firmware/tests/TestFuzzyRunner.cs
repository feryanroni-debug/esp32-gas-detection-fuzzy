using System;
using System.Collections.Generic;

namespace FuzzyTest
{
    public class FuzzyResult
    {
        public float CrispScore { get; set; }
        public string ConditionStr { get; set; }
        public float AlphaAman { get; set; }
        public float AlphaWaspada { get; set; }
        public float AlphaBahaya { get; set; }
    }

    public class FuzzyEngineSimulator
    {
        // 1. Fungsi Keanggotaan: Gas LPG (G) [0 - 5000 ppm]
        public static float MuGasAman(float g)
        {
            if (g <= 0.0f) return 1.0f;
            if (g < 1500.0f) return (1500.0f - g) / 1500.0f;
            return 0.0f;
        }

        public static float MuGasWaspada(float g)
        {
            if (g <= 500.0f || g >= 2500.0f) return 0.0f;
            if (g > 500.0f && g <= 1500.0f) return (g - 500.0f) / 1000.0f;
            return (2500.0f - g) / 1000.0f;
        }

        public static float MuGasBahaya(float g)
        {
            if (g <= 1500.0f) return 0.0f;
            if (g < 2500.0f) return (g - 1500.0f) / 1000.0f;
            return 1.0f;
        }

        // 2. Fungsi Keanggotaan: Asap (S) [0 - 1000 ppm]
        public static float MuAsapNormal(float s)
        {
            if (s <= 0.0f) return 1.0f;
            if (s < 300.0f) return (300.0f - s) / 300.0f;
            return 0.0f;
        }

        public static float MuAsapWaspada(float s)
        {
            if (s <= 200.0f || s >= 800.0f) return 0.0f;
            if (s > 200.0f && s <= 500.0f) return (s - 200.0f) / 300.0f;
            return (800.0f - s) / 300.0f;
        }

        public static float MuAsapBahaya(float s)
        {
            if (s <= 700.0f) return 0.0f;
            if (s < 1000.0f) return (s - 700.0f) / 300.0f;
            return 1.0f;
        }

        // 3. Fungsi Keanggotaan: Api (A) [0.0 - 1.0]
        public static float MuApiTidakAda(float a)
        {
            if (a <= 0.0f) return 1.0f;
            if (a < 0.5f) return (0.5f - a) / 0.5f;
            return 0.0f;
        }

        public static float MuApiTerdeteksi(float a)
        {
            if (a <= 0.25f || a >= 0.75f) return 0.0f;
            if (a > 0.25f && a <= 0.5f) return (a - 0.25f) / 0.25f;
            return (0.75f - a) / 0.25f;
        }

        public static float MuApiBesar(float a)
        {
            if (a <= 0.5f) return 0.0f;
            if (a < 1.0f) return (a - 0.5f) / 0.5f;
            return 1.0f;
        }

        // 4. Fungsi Keanggotaan Output: Kondisi Kebocoran (K) [0 - 100]
        public static float MuOutputAman(float k)
        {
            if (k <= 0.0f) return 1.0f;
            if (k < 50.0f) return (50.0f - k) / 50.0f;
            return 0.0f;
        }

        public static float MuOutputWaspada(float k)
        {
            if (k <= 25.0f || k >= 75.0f) return 0.0f;
            if (k > 25.0f && k <= 50.0f) return (k - 25.0f) / 25.0f;
            return (75.0f - k) / 25.0f;
        }

        public static float MuOutputBahaya(float k)
        {
            if (k <= 50.0f) return 0.0f;
            if (k < 100.0f) return (k - 50.0f) / 50.0f;
            return 1.0f;
        }

        private static float Min3(float a, float b, float c)
        {
            return Math.Min(a, Math.Min(b, c));
        }

        // 5. Evaluasi 27 Aturan Mamdani & Defuzzifikasi Centroid
        public static FuzzyResult Compute(float gasPpm, float smokePpm, float flameVal)
        {
            float uG_Aman = MuGasAman(gasPpm);
            float uG_Waspada = MuGasWaspada(gasPpm);
            float uG_Bahaya = MuGasBahaya(gasPpm);

            float uS_Normal = MuAsapNormal(smokePpm);
            float uS_Waspada = MuAsapWaspada(smokePpm);
            float uS_Bahaya = MuAsapBahaya(smokePpm);

            float uA_TidakAda = MuApiTidakAda(flameVal);
            float uA_Terdeteksi = MuApiTerdeteksi(flameVal);
            float uA_Besar = MuApiBesar(flameVal);

            float alphaAman = 0.0f;
            float alphaWaspada = 0.0f;
            float alphaBahaya = 0.0f;

            // R1: Aman, Normal, Tidak Ada -> Aman
            alphaAman = Math.Max(alphaAman, Min3(uG_Aman, uS_Normal, uA_TidakAda));
            // R2: Aman, Normal, Terdeteksi -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Aman, uS_Normal, uA_Terdeteksi));
            // R3: Aman, Normal, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Aman, uS_Normal, uA_Besar));

            // R4: Aman, Waspada, Tidak Ada -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Aman, uS_Waspada, uA_TidakAda));
            // R5: Aman, Waspada, Terdeteksi -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Aman, uS_Waspada, uA_Terdeteksi));
            // R6: Aman, Waspada, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Aman, uS_Waspada, uA_Besar));

            // R7: Aman, Bahaya, Tidak Ada -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Aman, uS_Bahaya, uA_TidakAda));
            // R8: Aman, Bahaya, Terdeteksi -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Aman, uS_Bahaya, uA_Terdeteksi));
            // R9: Aman, Bahaya, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Aman, uS_Bahaya, uA_Besar));

            // R10: Waspada, Normal, Tidak Ada -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Waspada, uS_Normal, uA_TidakAda));
            // R11: Waspada, Normal, Terdeteksi -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Waspada, uS_Normal, uA_Terdeteksi));
            // R12: Waspada, Normal, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Normal, uA_Besar));

            // R13: Waspada, Waspada, Tidak Ada -> Waspada
            alphaWaspada = Math.Max(alphaWaspada, Min3(uG_Waspada, uS_Waspada, uA_TidakAda));
            // R14: Waspada, Waspada, Terdeteksi -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Waspada, uA_Terdeteksi));
            // R15: Waspada, Waspada, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Waspada, uA_Besar));

            // R16: Waspada, Bahaya, Tidak Ada -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Bahaya, uA_TidakAda));
            // R17: Waspada, Bahaya, Terdeteksi -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Bahaya, uA_Terdeteksi));
            // R18: Waspada, Bahaya, Besar -> Bahaya
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Waspada, uS_Bahaya, uA_Besar));

            // R19 - R27: Gas Bahaya -> seluruhnya BAHAYA
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Normal, uA_TidakAda));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Normal, uA_Terdeteksi));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Normal, uA_Besar));

            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Waspada, uA_TidakAda));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Waspada, uA_Terdeteksi));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Waspada, uA_Besar));

            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Bahaya, uA_TidakAda));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Bahaya, uA_Terdeteksi));
            alphaBahaya = Math.Max(alphaBahaya, Min3(uG_Bahaya, uS_Bahaya, uA_Besar));

            // Defuzzifikasi Diskrit Centroid (CoG)
            float sumNumerator = 0.0f;
            float sumDenominator = 0.0f;

            for (int z = 0; z <= 100; z++)
            {
                float fz = (float)z;
                float muA = Math.Min(alphaAman, MuOutputAman(fz));
                float muW = Math.Min(alphaWaspada, MuOutputWaspada(fz));
                float muB = Math.Min(alphaBahaya, MuOutputBahaya(fz));

                float muGabungan = Math.Max(muA, Math.Max(muW, muB));
                sumNumerator += (fz * muGabungan);
                sumDenominator += muGabungan;
            }

            float crisp = (sumDenominator <= 0.00001f) ? 0.0f : (sumNumerator / sumDenominator);

            string condStr = "AMAN";
            if (crisp <= 35.0f) condStr = "AMAN";
            else if (crisp < 65.0f) condStr = "WASPADA";
            else condStr = "BAHAYA";

            return new FuzzyResult
            {
                CrispScore = crisp,
                ConditionStr = condStr,
                AlphaAman = alphaAman,
                AlphaWaspada = alphaWaspada,
                AlphaBahaya = alphaBahaya
            };
        }
    }

    class Program
    {
        static int Main(string[] args)
        {
            var testCases = new[]
            {
                // Skenario 1: Normal (Udara Bersih)
                new { Name = "Skenario 1.1: Udara Bersih Murni", Gas = 50f, Smoke = 20f, Flame = 0.0f, Expected = "AMAN" },
                new { Name = "Skenario 1.2: Normal Menengah", Gas = 250f, Smoke = 80f, Flame = 0.05f, Expected = "AMAN" },
                new { Name = "Skenario 1.3: Normal Ambang Atas", Gas = 450f, Smoke = 150f, Flame = 0.10f, Expected = "AMAN" },

                // Skenario 2: Gas Rendah / Waspada Awal
                new { Name = "Skenario 2.1: Gas Mulai Bocor", Gas = 800f, Smoke = 250f, Flame = 0.0f, Expected = "WASPADA" },
                new { Name = "Skenario 2.2: Gas Sedang & Asap Ringan", Gas = 1200f, Smoke = 350f, Flame = 0.15f, Expected = "WASPADA" },
                new { Name = "Skenario 2.3: Asap Waspada Tinggi", Gas = 400f, Smoke = 600f, Flame = 0.0f, Expected = "WASPADA" },

                // Skenario 3: Gas Tinggi / Kritis (> 1500 ppm)
                new { Name = "Skenario 3.1: Gas Tinggi (> 1500 ppm)", Gas = 2200f, Smoke = 300f, Flame = 0.0f, Expected = "BAHAYA" },
                new { Name = "Skenario 3.2: Kebocoran Gas Masif", Gas = 3000f, Smoke = 500f, Flame = 0.0f, Expected = "BAHAYA" },
                new { Name = "Skenario 3.3: Gas & Asap Maksimum", Gas = 4800f, Smoke = 800f, Flame = 0.0f, Expected = "BAHAYA" },

                // Skenario 4: Nyala Api Muncul
                new { Name = "Skenario 4.1: Api Kecil (Gas Rendah)", Gas = 200f, Smoke = 50f, Flame = 0.45f, Expected = "WASPADA" },
                new { Name = "Skenario 4.2: Api Signifikan Muncul", Gas = 400f, Smoke = 100f, Flame = 0.85f, Expected = "BAHAYA" },
                new { Name = "Skenario 4.3: Gas Bocor Disertai Api", Gas = 1500f, Smoke = 400f, Flame = 0.70f, Expected = "BAHAYA" }
            };

            Console.WriteLine("==========================================================================================");
            Console.WriteLine("       HASIL EKSEKUSI PENGUJIAN VALIDASI ALGORITMA FUZZY LOGIC MAMDANI (27 RULES)         ");
            Console.WriteLine("==========================================================================================");
            Console.WriteLine("{0,-36} {1,8} {2,8} {3,7} | {4,5} {5,5} {6,5} | {7,8} {8,-8} {9,-6}",
                "Kasus Uji Skenario", "Gas(ppm)", "Asap", "Api", "a_Am", "a_Wp", "a_Bh", "Skor(K)", "Status", "Hasil");
            Console.WriteLine(new string('-', 106));

            int passed = 0;
            foreach (var tc in testCases)
            {
                var res = FuzzyEngineSimulator.Compute(tc.Gas, tc.Smoke, tc.Flame);
                bool isPass = (res.ConditionStr == tc.Expected);
                if (isPass) passed++;

                Console.WriteLine("{0,-36} {1,8:F1} {2,8:F1} {3,7:F2} | {4,5:F2} {5,5:F2} {6,5:F2} | {7,8:F2} {8,-8} {9,-6}",
                    tc.Name, tc.Gas, tc.Smoke, tc.Flame,
                    res.AlphaAman, res.AlphaWaspada, res.AlphaBahaya,
                    res.CrispScore, res.ConditionStr, isPass ? "[PASS]" : "[FAIL]");
            }

            Console.WriteLine("==========================================================================================");
            Console.WriteLine("Total Pengujian : {0} Kasus Uji", testCases.Length);
            Console.WriteLine("Berhasil (PASS) : {0} Kasus Uji", passed);
            Console.WriteLine("Gagal (FAIL)    : {0} Kasus Uji", testCases.Length - passed);
            Console.WriteLine("Status Akhir    : {0}", (passed == testCases.Length) ? "SEMUA PENGUJIAN BERHASIL TERCAPAI 100%" : "ADA KASUS UJI GAGAL");
            Console.WriteLine("==========================================================================================");

            return (passed == testCases.Length) ? 0 : 1;
        }
    }
}
