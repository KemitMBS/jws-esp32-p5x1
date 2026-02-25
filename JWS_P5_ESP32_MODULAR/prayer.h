#ifndef PRAYER_H
#define PRAYER_H

#include "PrayerTimes.h"

class PrayerManager {
public:
    PrayerTimes* pt = nullptr;
    double pMnt[8];

    void init(float lat, float lon, int tz=7){
        if(pt) delete pt;
        pt = new PrayerTimes(lat, lon, tz); 
        pt->setCalculationMethod(CalculationMethods::INDONESIA);
        pt->setAsrMethod(SHAFII);
    }

    void updateJadwal(DateTime now){
        int h[8], m[8];
        pt->calculate(now.day(),now.month(),now.year(),
                      h[1],m[1],h[2],m[2],
                      h[4],m[4],h[5],m[5],
                      h[6],m[6],h[7],m[7]);
        // contoh simplifikasi, bisa ditambah NIH/kws
        for(int i=0;i<8;i++) pMnt[i] = h[i]*60+m[i];
    }
};

#endif