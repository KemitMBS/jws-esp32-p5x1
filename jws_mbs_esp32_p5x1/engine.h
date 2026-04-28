#ifndef ENGINE_H
#define ENGINE_H

#include "config_hw.h"
#include "SolarCalculator.h"

// Deklarasi variabel global dari main.ino (agar bisa diakses di sini)
extern PrayerTimes* pt;
extern float currentLat, currentLon;
extern int NIH;
extern int kws[8];
extern double pMnt[8];
extern long globalTafSec;
extern int nextEventIdx;
extern long nextEventSec;
extern float duhaAngle;
extern int imsakOffset;

void initPrayerObject() {
    if (pt != nullptr) delete pt;
    
    pt = new PrayerTimes(currentLat, currentLon, 420); 
    pt->setCalculationMethod(CalculationMethods::INDONESIA);
    pt->setAsrMethod(SHAFII);
    pt->setImsakOffset(imsakOffset); 
    pt->setDuhaAngle(duhaAngle);
}

void updateJadwal(DateTime now) {
    if (!pt) return;

    PrayerTimesResult result = pt->calculate(now.day(), now.month(), now.year());
    
    if(result.valid) {
        pMnt[0] = result.imsak + kws[0];           
        pMnt[1] = result.fajr + kws[1] + NIH;      
        pMnt[2] = result.sunrise + kws[2];         
        pMnt[3] = result.duha + kws[3];            
        pMnt[4] = result.dhuhr + kws[4] + NIH;     
        pMnt[5] = result.asr + kws[5] + NIH;       
        pMnt[6] = result.maghrib + kws[6] + NIH;   
        pMnt[7] = result.isha + kws[7] + NIH;      
    } else {
        Serial.println("Gagal menghitung jadwal: " + String(result.errorMessage));
    }

    double eot;
    calcEquationOfTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), eot);
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
