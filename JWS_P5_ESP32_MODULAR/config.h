// ===== config.h =====
#ifndef CONFIG_H
#define CONFIG_H

#include <EEPROM.h>

#define ADDR_KWS 10
#define ADDR_NIH 20
#define ADDR_LAT 25
#define ADDR_LON 30

struct Config {
    int kws[8];
    int NIH;
    float lat;
    float lon;
};

Config loadConfig() {
    Config cfg;
    cfg.NIH = EEPROM.read(ADDR_NIH);
    if(cfg.NIH > 10) cfg.NIH = 2;

    EEPROM.get(ADDR_LAT, cfg.lat);
    EEPROM.get(ADDR_LON, cfg.lon);
    if(isnan(cfg.lat)) cfg.lat = -6.9672;
    if(isnan(cfg.lon)) cfg.lon = 109.0650;

    for(int i=0;i<8;i++){
        int8_t v = (int8_t)EEPROM.read(ADDR_KWS+i);
        cfg.kws[i] = (abs(v)<=30)? v : 0;
    }
    return cfg;
}

void saveConfig(Config cfg){
    EEPROM.write(ADDR_NIH, cfg.NIH);
    EEPROM.put(ADDR_LAT, cfg.lat);
    EEPROM.put(ADDR_LON, cfg.lon);
    for(int i=0;i<8;i++){
        EEPROM.write(ADDR_KWS+i, (int8_t)cfg.kws[i]);
    }
    EEPROM.commit();
}

#endif