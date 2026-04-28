#ifndef SERIAL_H
#define SERIAL_H

#include "config_hw.h"
#include "engine.h"

// Helper: kirim respon ke Serial dan Bluetooth
void sendResponse(String msg) {
    Serial.println(msg);
    if (SerialBT.hasClient()) SerialBT.println(msg);
}

// Fungsi utama menangani perintah dari Serial / Bluetooth
void handleSerial() {   // ← parameter now dihapus, selalu ambil dari RTC
    String cmd = "";
    if (Serial.available() > 0) cmd = Serial.readStringUntil('\n');
    else if (SerialBT.available() > 0) cmd = SerialBT.readStringUntil('\n');

    if (cmd == "") return;
    cmd.trim();

    bool valid = false;
    pref.begin("jws_mbs", false);

    // -------------------- 1. Sinkronisasi RTC (SDTDDMMYYHHMMSS) --------------------
    if (cmd.startsWith("SDT")) {
        if (cmd.length() >= 15) {
            int tgl = cmd.substring(3, 5).toInt();
            int bln = cmd.substring(5, 7).toInt();
            int thn = cmd.substring(7, 9).toInt();
            int jam = cmd.substring(9, 11).toInt();
            int mnt = cmd.substring(11, 13).toInt();
            int dtk = cmd.substring(13, 15).toInt();

            if (thn >= 0 && thn <= 99 && bln >= 1 && bln <= 12 && tgl >= 1 && tgl <= 31 &&
                jam >= 0 && jam <= 23 && mnt >= 0 && mnt <= 59 && dtk >= 0 && dtk <= 59) {

                rtc.adjust(DateTime(thn + 2000, bln, tgl, jam, mnt, dtk));
                DateTime now = rtc.now();               // ambil waktu terbaru setelah adjust
                updateJadwal(now);
                updateDates(now);

                sendResponse("OK: Waktu Sinkron -> " + String(now.timestamp()));
                valid = true;
            } else {
                sendResponse("ERROR: Nilai tanggal/jam tidak valid");
            }
        } else {
            sendResponse("ERROR: Format harus SDTDDMMYYHHMMSS (15 karakter)");
        }
    }

    // -------------------- 2. Set Jam & Menit saja (SJHHMM) --------------------
    else if (cmd.startsWith("SJ")) {
        if (cmd.length() >= 6) {
            DateTime curr = rtc.now();   // ambil tanggal & tahun dari RTC
            int jam = cmd.substring(2, 4).toInt();
            int mnt = cmd.substring(4, 6).toInt();
            if (jam >= 0 && jam <= 23 && mnt >= 0 && mnt <= 59) {
                rtc.adjust(DateTime(curr.year(), curr.month(), curr.day(), jam, mnt, 0));
                updateJadwal(rtc.now());
                sendResponse("OK: Jam Diatur ke " + String(jam) + ":" + String(mnt));
                valid = true;
            } else {
                sendResponse("ERROR: Jam/menit tidak valid");
            }
        } else {
            sendResponse("ERROR: Format SJHHMM (6 karakter)");
        }
    }

    // -------------------- 3. Koreksi (NC0=Hijriyah, NC1..NC7=sholat) --------------------
    else if (cmd.startsWith("NC")) {
        if (cmd.length() >= 4) {
            int idx = cmd.substring(2, 3).toInt();
            int val = cmd.substring(3).toInt();

            // Validasi nilai batasan
            if (idx == 0) {
                // Koreksi Hijriyah (biasanya antara -3 .. +3)
                if (val >= -10 && val <= 10) {
                    hijriCorrection = val;
                    pref.putInt("hijri_corr", hijriCorrection);
                    updateJadwal(rtc.now());
                    DateTime now = rtc.now();
                    updateDates(now);
                    sendResponse("OK: Koreksi Hijriyah = " + String(hijriCorrection));
                    valid = true;
                } else {
                    sendResponse("ERROR: Koreksi Hijriyah di luar range (-10..10)");
                }
            }
            else if (idx >= 1 && idx <= 7) {
                // Koreksi waktu sholat (menit) - batasan -30..30 menit
                if (val >= -30 && val <= 30) {
                    kws[idx] = val;
                    pref.putInt(("kws" + String(idx)).c_str(), val);
                    updateJadwal(rtc.now());
                    sendResponse("OK: KWS " + pNama[idx] + " = " + String(val));
                    valid = true;
                } else {
                    sendResponse("ERROR: Koreksi waktu sholat di luar range (-30..30)");
                }
            }
            else {
                sendResponse("ERROR: Index NC harus 0 (Hijriyah) atau 1-7 (sholat)");
            }
        } else {
            sendResponse("ERROR: Format NC0nilai / NC1nilai ... NC7nilai");
        }
    }

    // -------------------- 4. NIH - Nilai Ihtiyati (menit) --------------------
    else if (cmd.startsWith("NIH")) {
        int val = cmd.substring(3).toInt();
        if (val >= 0 && val <= 30) {
            NIH = val;
            pref.putInt("nih", NIH);
            updateJadwal(rtc.now());
            sendResponse("OK: NIH diubah menjadi " + String(NIH) + " menit");
            valid = true;
        } else {
            sendResponse("ERROR: NIH harus antara 0..30 menit");
        }
    }

    // -------------------- 5. NLA - Latitude --------------------
    else if (cmd.startsWith("NLA")) {
        float val = cmd.substring(3).toFloat();
        if (val >= -90.0 && val <= 90.0) {
            currentLat = val;
            pref.putFloat("lat", currentLat);
            initPrayerObject();
            updateJadwal(rtc.now());
            sendResponse("OK: Latitude = " + String(currentLat, 6));
            valid = true;
        } else {
            sendResponse("ERROR: Latitude harus -90..90");
        }
    }

    // -------------------- 6. NLO - Longitude --------------------
    else if (cmd.startsWith("NLO")) {
        float val = cmd.substring(3).toFloat();
        if (val >= -180.0 && val <= 180.0) {
            currentLon = val;
            pref.putFloat("lon", currentLon);
            initPrayerObject();
            updateJadwal(rtc.now());
            sendResponse("OK: Longitude = " + String(currentLon, 6));
            valid = true;
        } else {
            sendResponse("ERROR: Longitude harus -180..180");
        }
    }

    // -------------------- 7. NDA - Sudut Duha --------------------
    else if (cmd.startsWith("NDA")) {
        float val = cmd.substring(3).toFloat();
        if (val >= 0.0 && val <= 90.0) {
            duhaAngle = val;
            pref.putFloat("duha_ang", duhaAngle);
            initPrayerObject();
            updateJadwal(rtc.now());
            sendResponse("OK: Sudut Duha = " + String(duhaAngle, 1));
            valid = true;
        } else {
            sendResponse("ERROR: Sudut Duha harus 0..90 derajat");
        }
    }

    // -------------------- 8. NIO - Imsak Offset (menit) --------------------
    else if (cmd.startsWith("NIO")) {
        int val = cmd.substring(3).toInt();
        if (val >= -30 && val <= 30) {
            imsakOffset = val;
            pref.putInt("imsak_off", imsakOffset);
            initPrayerObject();
            updateJadwal(rtc.now());
            sendResponse("OK: Offset Imsak = " + String(imsakOffset) + " menit");
            valid = true;
        } else {
            sendResponse("ERROR: Offset Imsak harus -30..30 menit");
        }
    }

    // -------------------- 9. CEK - Cek konfigurasi dasar --------------------
    else if (cmd == "CEK") {
        String msg = "LAT:" + String(currentLat, 4) +
                     "|LON:" + String(currentLon, 4) +
                     "|NIH:" + String(NIH) +
                     "|DHA:" + String(duhaAngle, 1) +
                     "|IMS:" + String(imsakOffset);
        sendResponse(msg);
        valid = true;
    }

    // -------------------- 10. JAD - Jadwal sholat lengkap + TANGGAL --------------------
    else if (cmd == "JAD") {
        DateTime now = rtc.now();
        updateDates(now);   // pastikan hijriDate terbaru

        // Tampilkan tanggal Masehi
        char tglBuf[20];
        sprintf(tglBuf, "%02d/%02d/%04d", now.day(), now.month(), now.year());
        sendResponse("Tanggal Masehi  : " + String(tglBuf));

        // Tampilkan tanggal Hijriyah
        sendResponse("Tanggal Hijriyah: " + hijriDate);

        sendResponse("=========================");
        for (int i = 0; i < 8; i++) {
            int h = (int)pMnt[i] / 60;
            int m = (int)pMnt[i] % 60;
            char buf[25];
            sprintf(buf, "%-8s : %02d:%02d", pNama[i].c_str(), h, m);
            sendResponse(String(buf));
        }
        sendResponse("=========================");
        String configMsg = "IHTIYAT:" + String(NIH) +
                           " | DHUHA:" + String(duhaAngle, 1) +
                           "° | IMSAK offset:" + String(imsakOffset) + "mnt";
        sendResponse(configMsg);
        valid = true;
    }

    // -------------------- 11. WIS - Waktu Istiwa (Tafawut) --------------------
    else if (cmd == "WIS") {
        sendResponse("WIS Offset: " + String(globalTafSec) + " detik");
        valid = true;
    }

    // -------------------- 12. TGL - Tampilkan tanggal Masehi saja --------------------
    else if (cmd == "TGL") {
        DateTime now = rtc.now();
        char buf[11];
        sprintf(buf, "%02d/%02d/%04d", now.day(), now.month(), now.year());
        sendResponse("Tanggal Masehi: " + String(buf));
        valid = true;
    }

    // -------------------- 13. HIJ - Tampilkan tanggal Hijriyah saja --------------------
    else if (cmd == "HIJ") {
        DateTime now = rtc.now();
        updateDates(now);
        sendResponse("Tanggal Hijriyah: " + hijriDate);
        valid = true;
    }

    pref.end();
    if (!valid) sendResponse("Invalid command: " + cmd);
}

#endif
