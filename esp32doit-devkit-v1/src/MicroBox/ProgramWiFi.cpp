/**
 *  @file ProgramWiFi.cpp
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2026
 * 
 *  @copyright
 *  Copyright (C) 2026, basyair7
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "MicroBox/software/ProgramWiFi"
#include "MicroBox/software/SysHandlers"

// WiFi接続中の場合、プログラムを実行する。
void ProgramWiFiClass::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    led_warning.off();
    Serial.println(F("\nConnection to AP Successfully"));
}

// WiFi接続が切断された場合、一定時間ごとに再接続を試みる。
void ProgramWiFiClass::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    static unsigned long LastMillis = 0;
    if ((unsigned long) (millis() - LastMillis) >= 15000L &&
        WiFi.status() != WL_CONNECTED)
    {
        LastMillis = millis();
        Serial.println(F("Disconnected from WiFi Access Point"));
        Serial.print(F("WiFi lost connection, Reason "));
        Serial.println(info.wifi_sta_disconnected.reason);
        WiFi.begin(
            this->__SSID_STA__.c_str(),
            this->__PASS_STA__.c_str()
        );
        led_warning.run(1500);
    }
}

// WiFi接続が成功した場合、IPアドレスを取得して表示する。
void ProgramWiFiClass::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    this->LOCALIPServer = WiFi.localIP().toString().c_str();
    Serial.println(F("WiFi Connected"));
    Serial.print(F("IP Address : "));
    Serial.println(this->LOCALIPServer);
}

// STAモードでプログラムを実行を行う。
void ProgramWiFiClass::wifi_mode_sta() {
    // WiFiのSTAモードに設定する。
    // WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_STA);
    this->Bootbtn_obj.begin();

    // 接続安定化のため、WiFiスリープを無効化する。
    WiFi.setSleep(false);

    // イベントハンドラの登録を行う。
    // WiFiStationConnected onEvent
    WiFi.onEvent(
        std::bind(
            &ProgramWiFiClass::WiFiStationConnected, this,
            std::placeholders::_1, 
            std::placeholders::_2
        ), ARDUINO_EVENT_WIFI_STA_CONNECTED
    );
    // WiFiGotIP onEvent
    WiFi.onEvent(
        std::bind(
            &ProgramWiFiClass::WiFiGotIP, this,
            std::placeholders::_1,
            std::placeholders::_2
        ), ARDUINO_EVENT_WIFI_STA_GOT_IP
    );
    // WiFiStationDisconnected onEvent
    WiFi.onEvent(
        std::bind(
            &ProgramWiFiClass::WiFiStationDisconnected, this,
            std::placeholders::_1,
            std::placeholders::_2
        ), ARDUINO_EVENT_WIFI_STA_DISCONNECTED
    );

    // WiFi接続を開始する。
    WiFi.begin(this->__SSID_STA__.c_str(), this->__PASS_STA__.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        this->Bootbtn_obj.ChangeWiFiMode();
        AutoChangeState::run();
        delay(500);
    }

    // RSSI
    Serial.print(F("\nRSSI: "));
    Serial.println(WiFi.RSSI());
}

// APモードでプログラムを実行を行う。
void ProgramWiFiClass::wifi_mode_ap() {
    // WiFiのAPモードに設定する。
    WiFi.mode(WIFI_AP);
    led_warning.run(1500);

    // APモードでWiFiを開始する。
    WiFi.softAP(
        this->__SSID_AP__.c_str(),
        this->__PASS_AP__.c_str()
    );

    // APモードでのIPアドレスを取得して表示する。
    this->LOCALIPServer = WiFi.softAPIP().toString().c_str();
    Serial.print(F("IP Address: "));
    Serial.println(this->LOCALIPServer);
    Serial.println();
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ProgramWiFi)
/**
 * @brief グローバルインスタンスの定義
 * このインスタンスは、ProgramWiFiクラスのグローバルオブジェクトとして定義されており、プログラム全体で使用されます。
 * これにより、ProgramWiFiクラスの機能を簡単に利用できるようになります。
 */
ProgramWiFiClass ProgramWiFi;
#endif