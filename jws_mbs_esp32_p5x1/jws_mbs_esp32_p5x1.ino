#include "config_hw.h"

// --- 1. Deklarasi Variabel Global ---
MatrixPanel_I2S_DMA *matrix = nullptr;
RTC_DS3231 rtc;
PrayerTimes* pt = nullptr;
BluetoothSerial SerialBT;
Preferences pref;

int NIH = 2; 
float currentLat = -6.9672, currentLon = 109.0650; 
float duhaAngle = 3.5;
int imsakOffset = 10;
int kws[8] = {0}; 
double pMnt[8];
long globalTafSec = 0;
int nextEventIdx = 0;
long nextEventSec = 0;
int detikLalu = -1; // Berubah dari menitLalu ke detikLalu agar tampilan "hidup"
String pNama[] = {"IMSAK","SUBUH","TERBIT","DHUHA","DZUHUR","ASHAR","MAGHRIB","ISYA"};

// --- 2. Include Header (WAJIB di bawah variabel global) ---
#include "engine.h"
#include "display.h"
#include "serial.h"

// --- 3. Setup ---
void setup() {
    Serial.begin(115200);
    SerialBT.begin("JWS-MBS-REV");

    // Load data dari memori ESP32
    pref.begin("jws_mbs", true);
    currentLat = pref.getFloat("lat", -6.9672);
    currentLon = pref.getFloat("lon", 109.0650);
    NIH = pref.getInt("nih", 2);
    duhaAngle = pref.getFloat("duha_ang", 3.5);
    imsakOffset = pref.getInt("imsak_off", 10);
    for(int i=0; i<8; i++) {
        kws[i] = pref.getInt(("kws"+String(i)).c_str(), 0);
    }
    pref.end();

    // Inisialisasi Panel LED
    HUB75_I2S_CFG mxConfig(WIDTH, HEIGHT, CHAIN);
    mxConfig.gpio.r1=R1_PIN; mxConfig.gpio.g1=G1_PIN; mxConfig.gpio.b1=B1_PIN;
    mxConfig.gpio.r2=R2_PIN; mxConfig.gpio.g2=G2_PIN; mxConfig.gpio.b2=B2_PIN;
    mxConfig.gpio.a=A_PIN;   mxConfig.gpio.b=B_PIN;   mxConfig.gpio.c=C_PIN; 
    mxConfig.gpio.d=D_PIN;   mxConfig.gpio.e=E_PIN;
    mxConfig.gpio.clk=CLK_PIN; mxConfig.gpio.lat=LAT_PIN; mxConfig.gpio.oe=OE_PIN;
    mxConfig.driver=HUB75_I2S_CFG::FM6126A;

    matrix = new MatrixPanel_I2S_DMA(mxConfig);
    matrix->begin();
    matrix->setBrightness8(10);

    // Inisialisasi Waktu & Jadwal
    rtc.begin();
    initPrayerObject(); // Dari engine.h
    updateJadwal(rtc.now());

    // Setup Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW); // Pastikan mati saat awal
}

// --- 4. Loop Utama ---
void loop() {
    DateTime now = rtc.now();
    
    // Cek Bluetooth/Serial Command setiap saat
    handleSerial(now);
    
    // Refresh Tampilan setiap DETIK agar format HH:MM:SS IB berjalan
    if(now.second() != detikLalu) {
        checkNextEvent(now);    // Update hitung mundur
        drawLayout();           // matrix->fillScreen(0)
        showClock(now);         // Tampilkan Baris 1-4 (WIB, WIS, Event, Countdown)
        
        // Update jadwal harian tepat jam 00:00:00
        if(now.hour() == 0 && now.minute() == 0 && now.second() == 0) {
            updateJadwal(now);
        }
        
        detikLalu = now.second();
    }
}