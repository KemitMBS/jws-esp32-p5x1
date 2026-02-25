#ifndef DISPLAY_H
#define DISPLAY_H

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <RTClib.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/TomThumb.h>

enum ModeDisplay { MODE_NORMAL, MODE_ADZAN };

class DisplayManager {
public:
    MatrixPanel_I2S_DMA* matrix;
    ModeDisplay mode = MODE_NORMAL;
    unsigned long modeStart = 0, lastBlink = 0;
    bool blinkState = false;
    int detikLalu = -1;

    DisplayManager(MatrixPanel_I2S_DMA* mtx) { matrix = mtx; }

    void showTime(DateTime now, float lon, double pMnt[8], String pNama[8]){
        long wibSec = now.hour()*3600L + now.minute()*60L + now.second();

        // WIS ultra-presisi
        double tawafutSec = (lon - 105.0) * 240.0; // 4 menit per derajat
        double wisExact = fmod((double)wibSec + tawafutSec + 86400.0, 86400.0);

        int wisJam = (int)(wisExact / 3600);
        int wisMenit = (int)(wisExact / 60) % 60;
        int wisDetik = (int)wisExact % 60;

        // Cari sholat berikutnya
        int targetIdx = 0;
        bool found = false;
        for(int i = 0; i < 8; i++){
            if(pMnt[i]*60.0 > wibSec){
                targetIdx = i;
                found = true;
                break;
            }
        }
        long sisaDetik = found ? (pMnt[targetIdx]*60 - wibSec) : (pMnt[0]*60 + 86400 - wibSec);

        if(now.second() != detikLalu){
            matrix->fillScreen(0);

            // WIB
            matrix->setFont(&FreeSansBold9pt7b);
            matrix->setTextColor(matrix->color565(0,255,255));
            matrix->setCursor(2,14);
            matrix->printf("%02d:%02d", now.hour(), now.minute());

            matrix->setFont(&TomThumb);
            matrix->setTextColor(matrix->color565(255,200,0));
            matrix->setCursor(48,8);
            matrix->printf("%02d", now.second());
            matrix->setTextColor(matrix->color565(200,200,0));
            matrix->setCursor(48,14);
            matrix->print("WIB");

            // WIS
            matrix->setFont(&FreeSansBold9pt7b);
            matrix->setTextColor(matrix->color565(0,255,0));
            matrix->setCursor(2,30);
            matrix->printf("%02d:%02d", wisJam, wisMenit);

            matrix->setFont(&TomThumb);
            matrix->setTextColor(matrix->color565(255,200,0));
            matrix->setCursor(48,24);
            matrix->printf("%02d", wisDetik);
            matrix->setTextColor(matrix->color565(200,200,0));
            matrix->setCursor(48,30);
            matrix->print("WIS");

            // Sholat info
            matrix->setFont(&TomThumb);
            matrix->setTextColor(matrix->color565(255,255,255));
            matrix->setCursor(68,12);
            matrix->print(pNama[targetIdx]);
            int h=sisaDetik/3600, m=(sisaDetik/60)%60, s=sisaDetik%60;
            matrix->setTextColor(matrix->color565(255,200,0));
            matrix->setCursor(68,24);
            matrix->printf("-%02d:%02d:%02d", h,m,s);

            detikLalu = now.second();
        }

        // Mode ADZAN
        if(found && sisaDetik<=0){ 
            mode = MODE_ADZAN; 
            modeStart = millis(); 
        }
        else if(mode == MODE_ADZAN){
            if(millis()-modeStart>=60000){ mode = MODE_NORMAL; return; }
            if(millis()-lastBlink>=500){
                lastBlink = millis(); 
                blinkState = !blinkState; 
                matrix->fillScreen(0);
                if(blinkState){
                    matrix->setFont(&FreeSansBold9pt7b);
                    matrix->setTextColor(matrix->color565(255,0,0));
                    matrix->setCursor(35,20); 
                    matrix->print("ADZAN");
                }
            }
        }
    }
};

#endif