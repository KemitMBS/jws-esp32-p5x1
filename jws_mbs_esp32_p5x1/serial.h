#ifndef SERIAL_H
#define SERIAL_H

#include "config_hw.h"
#include "engine.h"

// Fungsi helper untuk mengirim balik status ke Serial Monitor atau Bluetooth HP
void sendResponse(String msg){
    Serial.println(msg);
    if(SerialBT.hasClient()) SerialBT.println(msg);
}

void handleSerial(DateTime now) {
    String cmd = "";
    if (Serial.available() > 0) cmd = Serial.readStringUntil('\n');
    else if (SerialBT.available() > 0) cmd = SerialBT.readStringUntil('\n');
    
    if (cmd == "") return; 
    cmd.trim();

    bool valid = false;
    pref.begin("jws_mbs", false); 

    // 1. Sinkronisasi Tanggal & Jam (Format: SDTYYMMDDHHMMSS)
    if (cmd.startsWith("SDT")) {
        if (cmd.length() >= 15) {
            rtc.adjust(DateTime(
                cmd.substring(3, 5).toInt() + 2000, 
                cmd.substring(5, 7).toInt(), 
                cmd.substring(7, 9).toInt(), 
                cmd.substring(9, 11).toInt(), 
                cmd.substring(11, 13).toInt(), 
                cmd.substring(13, 15).toInt()
            ));
            updateJadwal(rtc.now());
            sendResponse("OK: Waktu Sinkron");
            valid = true;
        }
    }
    // 2. Set Jam & Menit saja (Format: SJHHMM)
    else if (cmd.startsWith("SJ")) {
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                   cmd.substring(2, 4).toInt(), 
                   cmd.substring(4, 6).toInt(), 0));
        updateJadwal(rtc.now());
        sendResponse("OK: Jam Diatur");
        valid = true;
    }
    // 3. KWS - Koreksi Waktu Sholat (Format: NC[index][menit])
    else if (cmd.startsWith("NC")) { 
        int idx = cmd.substring(2, 3).toInt();
        int val = cmd.substring(3).toInt();
        if (idx >= 0 && idx <= 7) {
            kws[idx] = val;
            pref.putInt(("kws" + String(idx)).c_str(), val);
            updateJadwal(rtc.now());
            sendResponse("OK: KWS " + pNama[idx] + " Berhasil");
            valid = true;
        }
    }
    // 4. NIH - Nilai Ihtiyati (Format: NIH[menit])
    else if (cmd.startsWith("NIH")) {
        NIH = cmd.substring(3).toInt();
        pref.putInt("nih", NIH);
        updateJadwal(rtc.now());
        sendResponse("OK: NIH diubah");
        valid = true;
    }
    // 5. NLA - Latitude (Format: NLA-6.1234)
    else if (cmd.startsWith("NLA")) {
        currentLat = cmd.substring(3).toFloat();
        pref.putFloat("lat", currentLat);
        initPrayerObject();
        updateJadwal(rtc.now());
        sendResponse("OK: Lat Diubah");
        valid = true;
    }
    // 6. NLO - Longitude (Format: NLO106.1234)
    else if (cmd.startsWith("NLO")) {
        currentLon = cmd.substring(3).toFloat();
        pref.putFloat("lon", currentLon);
        initPrayerObject();
        updateJadwal(rtc.now());
        sendResponse("OK: Lon Diubah");
        valid = true;
    }
    // 7. NDA - Sudut Duha (Format: NDA3.5)
    else if (cmd.startsWith("NDA")) {
        duhaAngle = cmd.substring(3).toFloat();
        pref.putFloat("duha_ang", duhaAngle);
        initPrayerObject();
        updateJadwal(rtc.now());
        sendResponse("OK: Sudut Duha " + String(duhaAngle));
        valid = true;
    }
    // 8. NIO - Imsak Offset (Format: NIO12)
    else if (cmd.startsWith("NIO")) {
        imsakOffset = cmd.substring(3).toInt();
        pref.putInt("imsak_off", imsakOffset);
        initPrayerObject();
        updateJadwal(rtc.now());
        sendResponse("OK: Offset Imsak " + String(imsakOffset));
        valid = true;
    }
    // 9. CEK - Debugging Data Dasar
    else if (cmd == "CEK") {
        String msg = "LAT:" + String(currentLat, 4) + 
                     "|LON:" + String(currentLon, 4) + 
                     "|NIH:" + String(NIH) + 
                     "|DHA:" + String(duhaAngle, 1) + 
                     "|IMS:" + String(imsakOffset);
        sendResponse(msg);
        valid = true;
    }
    // 10. JAD - List Jadwal Sholat Lengkap
    else if (cmd == "JAD") {
        sendResponse("=========================");
        for (int i = 0; i < 8; i++) {
            int h = (int)pMnt[i] / 60;
            int m = (int)pMnt[i] % 60;
            char buf[25];
            sprintf(buf, "%-8s : %02d:%02d", pNama[i].c_str(), h, m);
            sendResponse(String(buf));
        }
        sendResponse("=========================");
        String configMsg = "|IHTIYAT:" + String(NIH) + 
                           "|DHUHA DERAJAT:" + String(duhaAngle, 1) + 
                           "|IMSAK:" + String(imsakOffset);
        sendResponse(configMsg);
        valid = true;
    }

    pref.end(); 
    if (!valid) sendResponse("Invalid: " + cmd);
}

#endif