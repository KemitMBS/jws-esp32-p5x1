#ifndef ENGINE_H
#define ENGINE_H

#include "config_hw.h"
#include "SolarCalculator.h" // Pastikan library ini sudah terinstall

// Variabel Global (Didefinisikan di .ino)
extern float duhaAngle;
extern int imsakOffset;

void initPrayerObject() {
    if (pt != nullptr) delete pt;
    
    // Constructor v2.1: (latitude, longitude, timezoneOffsetMinutes)
    pt = new PrayerTimes(currentLat, currentLon, 420); 
    
    // Set standar perhitungan Indonesia
    pt->setCalculationMethod(CalculationMethods::INDONESIA);
    pt->setAsrMethod(SHAFII);
    
    // Parameter Khusus v2.1
    pt->setImsakOffset(imsakOffset); 
    pt->setDuhaAngle(duhaAngle);
}

void updateJadwal(DateTime now) {
    if (!pt) return;

    // 1. Kalkulasi Jadwal Sholat (PrayerTimes v2.1)
    PrayerTimesResult result = pt->calculate(now.day(), now.month(), now.year());
    
    if(result.valid) {
        // Mapping hasil ke array pMnt (ditambah KWS & NIH)
        pMnt[0] = result.imsak + kws[0]; // KWS adalah koreksi waktu sholat           
        pMnt[1] = result.fajr + kws[1] + NIH; // NIH adalah Nilai ihtiyat watu sholat
        pMnt[2] = result.sunrise + kws[2];         
        pMnt[3] = result.duha + kws[3];            
        pMnt[4] = result.dhuhr + kws[4] + NIH;     
        pMnt[5] = result.asr + kws[5] + NIH;       
        pMnt[6] = result.maghrib + kws[6] + NIH;   
        pMnt[7] = result.isha + kws[7] + NIH;      
    } else {
        Serial.println("Gagal menghitung jadwal: " + String(result.errorMessage));
    }

    // 2. Kalkulasi Koreksi Waktu Istiwa (WIS) menggunakan SolarCalculator
    // Algoritma Jean Meeus untuk akurasi tinggi
    double eot;
    calcEquationOfTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), eot);
    
    // globalTafSec (dalam detik) = (Selisih Bujur + Equation of Time)
    // (currentLon - 105.0) * 4.0 adalah selisih menit bujur
    // eot adalah perataan waktu dalam menit
    // Hasil dikali 60.0 untuk mendapatkan total detik presisi
    globalTafSec = round(((currentLon - 105.0) * 4.0 + eot) * 60.0);
    
    Serial.print("WIS Offset (detik): ");
    Serial.println(globalTafSec);
}

void checkNextEvent(DateTime now) {
    long wibSec = now.hour() * 3600L + now.minute() * 60L + now.second();
    nextEventIdx = -1;

    for(int i = 0; i < 8; i++){
        if((long)(pMnt[i] * 60) > wibSec){
            nextEventIdx = i;
            nextEventSec = (long)(pMnt[i] * 60) - wibSec;
            break;
        }
    }

    if(nextEventIdx == -1){ 
        nextEventIdx = 0; 
        nextEventSec = (long)(pMnt[0] * 60) - wibSec + 86400L; 
    }
}


#endif
