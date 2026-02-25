#ifndef DISPLAY_H
#define DISPLAY_H
#include "config_hw.h"

// Pastikan di config_hw.h atau main file sudah ada: #define BUZZER_PIN 32

void drawLayout() {
    matrix->fillScreen(0);
}

void showClock(DateTime now) {
    // BARIS 1: WIB (B) - Warna Cyan
    matrix->setTextColor(matrix->color565(0, 255, 255));
    matrix->setCursor(2, 1); 
    matrix->printf("%02d:%02d:%02d B", now.hour(), now.minute(), now.second());

    // BARIS 2: WIS (S) - Warna Hijau
    long totalWIB = now.hour()*3600L + now.minute()*60L + now.second();
    long totalWIS = (totalWIB + globalTafSec + 86400L) % 86400L;
    int hWIS = (int)(totalWIS/3600);
    int mWIS = (int)((totalWIS%3600)/60);
    int sWIS = (int)(totalWIS%60);

    matrix->setTextColor(matrix->color565(0, 255, 0));
    matrix->setCursor(2, 9);
    matrix->printf("%02d:%02d:%02d S", hWIS, mWIS, sWIS);

    // BARIS 3: NAMA NEXT EVENT / TFW (Rolling 4 Detik Kontinu) - Warna Putih
    matrix->setTextColor(matrix->color565(255, 255, 255));
    
    // Logika Rolling: 4 detik Nama Sholat, 4 detik TFW (Berlaku semua event)
    if (now.second() % 8 >= 4) {
        long tfwAbs = abs(globalTafSec);
        int mTFW = tfwAbs / 60;
        int sTFW = tfwAbs % 60;
        char sign = (globalTafSec > 0) ? '-' : '+'; 

        matrix->setCursor(2, 17);
        matrix->printf("TFW %c%02d:%02d", sign, mTFW, sTFW);
    } else {
        int xNama = (64 - (pNama[nextEventIdx].length() * 6)) / 2; 
        matrix->setCursor(xNama, 17);
        matrix->print(pNama[nextEventIdx]);
    }

    // BARIS 4: COUNTDOWN - Warna Merah
    int h = (nextEventSec / 3600);
    int m = (nextEventSec % 3600) / 60;
    int s = nextEventSec % 60;
    
    matrix->setTextColor(matrix->color565(255, 0, 0));
    matrix->setCursor(5, 25); 
    matrix->printf("-%02d:%02d:%02d", h, m, s);

    // --- LOGIKA BUZZER PIN 32 ---
    // Bunyi saat masuk waktu (countdown 0)
    if (nextEventSec == 0) {
        digitalWrite(32, HIGH); 
    } else {
        digitalWrite(32, LOW);
    }
}

void showPrayerInfo() { }

#endif