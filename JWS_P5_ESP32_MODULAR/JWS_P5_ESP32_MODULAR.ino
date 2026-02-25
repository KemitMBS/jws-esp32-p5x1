#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <RTClib.h>
#include "display.h"
#include "serial.h"
#include "prayer.h"
#include "config.h"

RTC_DS3231 rtc;
HUB75_I2S_CFG mxConfig(64,32,2);
MatrixPanel_I2S_DMA matrix(mxConfig);

Config cfg;
PrayerManager prayer;
DisplayManager display(&matrix);

String pNama[] = {"IMSAK","SUBUH","TERBIT","DHUHA","DZUHUR","ASHAR","MAGHRIB","ISYA"};

void setup(){
    Serial.begin(115200);
    matrix.begin();
    matrix.setBrightness8(30);

    cfg.lat = -6.9672;
    cfg.lon = 109.0650;
    cfg.NIH = 2;

    prayer.init(cfg.lat, cfg.lon);
    prayer.updateJadwal(rtc.now());
}

void loop(){
    DateTime now = rtc.now();
    handleSerial(rtc, cfg, prayer);
    display.showTime(now, cfg.lon, prayer.pMnt, pNama);
}