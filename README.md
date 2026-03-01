# JWS MBS

Menggunakan esp32 untuk mendrive panel P5 

### Format tampilan :
_____________________________
01:23:45 WIB
01:23:45 WIS
IMSAK/TAFAWUTS -03:21 MENIT
-01:23:45
_____________________________

Untuk driver Panel P5 dan memakai PIN sesuai Shield JWS Kholifah esp32 30 pin
1. [Matrix RGB Display](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA) v3.0.13

Perhitungan Waktu sholat yang telah lengkap secara library untuk Imsak, Terbit dan Dhuha
2. [Prayer Times](https://github.com/a-saab/PrayerTimes) v2.1

Perhitungan waktu WIS / Waktu Istiwa / Solar Time secara akurat Sampai detik
3. [Solar Calculator](https://github.com/jpb10/SolarCalculator) v2.0.2

RTC
4. [RTClib](https://github.com/adafruit/RTClib)


Konfigurasi Pin Shield Kholifah :
```
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
```

Dapat memakai aplikasi android NZ TECH JWS via bluethoot dan hanya dapat digunakan dimenu Kordinat, Waktu dan Koreksi. untuk menu lainya belum saya masukan.
Source Kode JWS MBS ini adalah kode dasar yang dapat dikembangkan sesuai kebutuhan.

Karena Modular, maka tinggal dioprek tampilanya di display.h
