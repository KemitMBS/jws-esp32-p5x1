#ifndef DISPLAY_H
#define DISPLAY_H

#include "config_hw.h"
#include "hijri.h"   // diperlukan untuk tipe Hijriyah

// ================= DEKLARASI VARIABEL GLOBAL (dari main.ino) =================
extern Hijriyah hijriConverter;
extern int hijriCorrection;
extern long globalTafSec;
extern int nextEventIdx;
extern long nextEventSec;
extern String pNama[8];
extern bool showDateMode;
extern unsigned long lastModeChange;
extern String masehiDate;   // definisi ada di main.ino
extern String hijriDate;    // definisi ada di main.ino

// Durasi tampilan (dalam milidetik)
const unsigned long JAM_DURASI   = 25000;   // 25 detik jam
const unsigned long TANGGAL_DURASI = 5000;  // 5 detik tanggal

// ================= FUNGSI UPDATE TANGGAL =================
void updateDates(DateTime now) {
    char buf[11];
    sprintf(buf, "%02d/%02d/%04d", now.day(), now.month(), now.year());
    masehiDate = String(buf);

    // Gunakan hijriCorrection (bukan 0)
    hijriConverter.Update(now.year(), now.month(), now.day(), hijriCorrection);
    
    sprintf(buf, "%02d/%02d/%04d", 
            hijriConverter.getHijriyahDate, 
            hijriConverter.getHijriyahMonth, 
            hijriConverter.getHijriyahYear);
    hijriDate = String(buf);
}

// ================= FUNGSI UTAMA DISPLAY =================
void showClock(DateTime now) {
    unsigned long nowMs = millis();
    if (!showDateMode && (nowMs - lastModeChange >= JAM_DURASI)) {
        showDateMode = true;
        lastModeChange = nowMs;
    } else if (showDateMode && (nowMs - lastModeChange >= TANGGAL_DURASI)) {
        showDateMode = false;
        lastModeChange = nowMs;
    }

    matrix->fillScreen(0);

    if (!showDateMode) {
        // Mode jam (WIB dan WIS)
        long totalWIB = now.hour()*3600L + now.minute()*60L + now.second();
        long totalWIS = (totalWIB + globalTafSec + 86400L) % 86400L;
        int hWIS = totalWIS/3600;
        int mWIS = (totalWIS%3600)/60;
        int sWIS = totalWIS%60;

        matrix->setTextColor(matrix->color565(0, 255, 255));
        matrix->setCursor(2, 0);
        matrix->printf("%02d:%02d:%02d B", now.hour(), now.minute(), now.second());

        matrix->setTextColor(matrix->color565(0, 255, 0));
        matrix->setCursor(2, 8);
        matrix->printf("%02d:%02d:%02d S", hWIS, mWIS, sWIS);
    } else {
        // Mode tanggal
        matrix->setTextColor(matrix->color565(255, 255, 0));
        int lebarMasehi = masehiDate.length() * 6;
        int xMasehi = (64 - lebarMasehi) / 2;
        if (xMasehi < 0) xMasehi = 0;
        matrix->setCursor(xMasehi, 0);
        matrix->print(masehiDate);

        matrix->setTextColor(matrix->color565(0, 255, 255));
        int lebarHijri = hijriDate.length() * 6;
        int xHijri = (64 - lebarHijri) / 2;
        if (xHijri < 0) xHijri = 0;
        matrix->setCursor(xHijri, 8);
        matrix->print(hijriDate);
    }

    // Baris 3 - Nama event / TFW
    matrix->setTextColor(matrix->color565(255, 255, 255));
    if (now.second() % 8 >= 4) {
        long tfwAbs = abs(globalTafSec);
        int mTFW = tfwAbs / 60;
        int sTFW = tfwAbs % 60;
        char sign = (globalTafSec > 0) ? '-' : '+';
        matrix->setCursor(2, 16);
        matrix->printf("TFW %c%02d:%02d", sign, mTFW, sTFW);
    } else {
        int xNama = (64 - (pNama[nextEventIdx].length() * 6)) / 2;
        if (xNama < 0) xNama = 0;
        matrix->setCursor(xNama, 16);
        matrix->print(pNama[nextEventIdx]);
    }

    // Baris 4 - Countdown
    int h = nextEventSec / 3600;
    int m = (nextEventSec % 3600) / 60;
    int s = nextEventSec % 60;
    matrix->setTextColor(matrix->color565(255, 0, 0));
    matrix->setCursor(5, 24);
    matrix->printf("-%02d:%02d:%02d", h, m, s);

    digitalWrite(BUZZER_PIN, (nextEventSec == 0) ? HIGH : LOW);
}

#endif
