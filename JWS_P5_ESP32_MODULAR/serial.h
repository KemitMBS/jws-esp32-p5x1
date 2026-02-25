#ifndef SERIAL_H
#define SERIAL_H

#include <RTClib.h>
#include <EEPROM.h>
#include "prayer.h" // nanti buat class PrayerManager
#include "config.h" // nanti buat class Config

void handleSerial(RTC_DS3231 &rtc, Config &cfg, PrayerManager &pt){
    String cmd = "";
    if(Serial.available()) cmd = Serial.readStringUntil('\n');
    if(cmd=="") return; cmd.trim();

    if(cmd.startsWith("SJ")){ 
        DateTime now = rtc.now();
        rtc.adjust(DateTime(now.year(),now.month(),now.day(),
                            cmd.substring(2,4).toInt(),
                            cmd.substring(4,6).toInt(),0));
        pt.updateJadwal(rtc.now());
    }
    else if(cmd.startsWith("NLA")){
        cfg.lat = cmd.substring(3).toFloat();
        EEPROM.put(ADDR_LAT, cfg.lat);
        pt.init(cfg.lat, cfg.lon);
        pt.updateJadwal(rtc.now());
    }
    else if(cmd.startsWith("NLO")){
        cfg.lon = cmd.substring(3).toFloat();
        EEPROM.put(ADDR_LON, cfg.lon);
        pt.init(cfg.lat, cfg.lon);
        pt.updateJadwal(rtc.now());
    }
    else if(cmd.startsWith("NIH")){
        cfg.NIH = cmd.substring(3).toInt();
        EEPROM.write(ADDR_NIH, cfg.NIH);
        pt.updateJadwal(rtc.now());
    }
}
#endif