#ifndef HIJRI_H
#define HIJRI_H

#include "Arduino.h"
#include <math.h>

class Hijriyah {
  private:
    const float lunarY = 354.367068; // Rata-rata hari dalam satu tahun Hijriah
    
    // Menghitung jumlah hari Gregorian sejak epoch tertentu
    long Days(uint16_t YEAR, uint16_t MONTH, uint16_t DATE) {
      if (MONTH < 3) {
        YEAR -= 1;
        MONTH += 12;
      }
      YEAR = YEAR - 2000;
      long ndays = floor(365.25 * YEAR) + floor(30.6001 * (MONTH + 1)) + floor(YEAR / 100) + floor(YEAR / 400) + DATE + 196;
      return ndays;
    }
    
    // Menghitung jumlah hari Hijriah sejak epoch 1420 H
    long DaysHijri(uint16_t YEAR, uint16_t MONTH, uint16_t DATE) {
      YEAR = YEAR - 1420;
      long hari = floor(29.5 * MONTH - 28.999) + floor(lunarY * YEAR) + DATE;
      return hari;
    }
    
  public:
    uint16_t getHijriyahDate;
    uint16_t getHijriyahMonth;
    uint16_t getHijriyahYear;

    // Fungsi utama untuk memperbarui tanggal Hijriah
    void Update(uint16_t YEAR, uint16_t MONTH, uint16_t DATE, int CORRECTION) {
      long nday = Days(YEAR, MONTH, DATE) + (1 + CORRECTION);
      
      // Loop untuk mencari tahun Hijriah
      long tahun = floor(nday / lunarY) + 1420;
      while (DaysHijri(tahun, 1, 1) <= nday) {
        tahun++;
      };
      tahun--;
      
      // Loop untuk mencari bulan Hijriah
      long bulan = 1;
      while (DaysHijri(tahun, bulan, 1) <= nday) {
        bulan++;
      };
      bulan--;
      
      // Hitung hari Hijriah
      long harike = 1 + nday - DaysHijri(tahun, bulan, 1);
      
      // Koreksi jika bulan ke-13
      if (bulan == 13) {
        bulan = 12;
        harike += 29;
      };
      
      getHijriyahDate = harike;
      getHijriyahMonth = bulan;
      getHijriyahYear = tahun;
    }
};

#endif
