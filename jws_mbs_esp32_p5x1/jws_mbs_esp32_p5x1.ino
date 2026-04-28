#include "config_hw.h"
#include "hijri.h"

// --- Deklarasi Variabel Global ---
MatrixPanel_I2S_DMA *matrix = nullptr;
RTC_DS3231 rtc;
PrayerTimes* pt = nullptr;
BluetoothSerial SerialBT;
Preferences pref;
Hijriyah hijriConverter;

int hijriCorrection = 0;          // koreksi manual Hijriyah

int NIH = 2; 
float currentLat = -6.9672, currentLon = 109.0650; 
float duhaAngle = 3.5;
int imsakOffset = 10;
int kws[8] = {0}; 
double pMnt[8];
long globalTafSec = 0;
int nextEventIdx = 0;
long nextEventSec = 0;
int detikLalu = -1;
String pNama[] = {"IMSAK","SUBUH","TERBIT","DHUHA","DZUHUR","ASHAR","MAGHRIB","ISYA"};
String masehiDate = "01/01/26";
String hijriDate = "01/01/1447";

bool showDateMode = false;
unsigned long lastModeChange = 0;

#include "engine.h"
#include "display.h"
#include "serial.h"

void setup() {
    Serial.begin(115200);
    SerialBT.begin("JWS-MBS-REV");

    pref.begin("jws_mbs", true);
    hijriCorrection = pref.getInt("hijri_corr", 0);
    currentLat = pref.getFloat("lat", -6.9672);
    currentLon = pref.getFloat("lon", 109.0650);
    NIH = pref.getInt("nih", 2);
    duhaAngle = pref.getFloat("duha_ang", 3.5);
    imsakOffset = pref.getInt("imsak_off", 10);
    for(int i=0; i<8; i++) {
        kws[i] = pref.getInt(("kws"+String(i)).c_str(), 0);
    }
    pref.end();

    HUB75_I2S_CFG mxConfig(WIDTH, HEIGHT, CHAIN);
    mxConfig.gpio.r1=R1_PIN; mxConfig.gpio.g1=G1_PIN; mxConfig.gpio.b1=B1_PIN;
    mxConfig.gpio.r2=R2_PIN; mxConfig.gpio.g2=G2_PIN; mxConfig.gpio.b2=B2_PIN;
    mxConfig.gpio.a=A_PIN;   mxConfig.gpio.b=B_PIN;   mxConfig.gpio.c=C_PIN; 
    mxConfig.gpio.d=D_PIN;   mxConfig.gpio.e=E_PIN;
    mxConfig.gpio.clk=CLK_PIN; mxConfig.gpio.lat=LAT_PIN; mxConfig.gpio.oe=OE_PIN;
    mxConfig.driver=HUB75_I2S_CFG::FM6126A;
    mxConfig.double_buff = true;

    matrix = new MatrixPanel_I2S_DMA(mxConfig);
    matrix->begin();
    matrix->setBrightness8(10);

    rtc.begin();
    initPrayerObject();
    updateJadwal(rtc.now());

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    showDateMode = false;
    lastModeChange = millis();
}

void loop() {
    DateTime now = rtc.now();
    unsigned long nowMs = millis();

    handleSerial(now);

    static int lastSecond = -1;
    if (now.second() != lastSecond) {
        updateDates(now);
        lastSecond = now.second();
    }

    const unsigned long JAM_DURASI   = 25000;
    const unsigned long TANGGAL_DURASI = 5000;

    if (!showDateMode && (nowMs - lastModeChange >= JAM_DURASI)) {
        showDateMode = true;
        lastModeChange = nowMs;
    } else if (showDateMode && (nowMs - lastModeChange >= TANGGAL_DURASI)) {
        showDateMode = false;
        lastModeChange = nowMs;
    }

    if (now.second() != detikLalu) {
        checkNextEvent(now);
        showClock(now);
        matrix->flipDMABuffer();
        matrix->clearScreen();
        delay(1);

        if (now.hour() == 0 && now.minute() == 0 && now.second() == 0) {
            updateJadwal(now);
        }

        detikLalu = now.second();
    }
}
