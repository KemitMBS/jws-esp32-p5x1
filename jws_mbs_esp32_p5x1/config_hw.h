#ifndef CONFIG_HW_H
#define CONFIG_HW_H

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <RTClib.h>
#include <BluetoothSerial.h>
#include <Preferences.h>
#include "PrayerTimes.h"

#define R1_PIN 19
#define G1_PIN 13
#define B1_PIN 18
#define R2_PIN 5
#define G2_PIN 12
#define B2_PIN 17
#define A_PIN 16
#define B_PIN 14
#define C_PIN 4
#define D_PIN 27
#define E_PIN -1
#define CLK_PIN 2
#define LAT_PIN 26
#define OE_PIN 15
#define WIDTH 64
#define HEIGHT 32
#define CHAIN 1
#define BUZZER_PIN 32 

extern MatrixPanel_I2S_DMA *matrix;
extern RTC_DS3231 rtc;
extern PrayerTimes* pt;
extern BluetoothSerial SerialBT;
extern Preferences pref;
extern int NIH;
extern float currentLat, currentLon;
extern int kws[8];
extern double pMnt[8];
extern long globalTafSec;
extern int nextEventIdx;
extern long nextEventSec;
extern String pNama[8];

#endif