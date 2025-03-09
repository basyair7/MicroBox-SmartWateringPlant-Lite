/**
 *  @file ProgramWiFi.cpp
 *  @version 1.0.0
 *  @author basyair7
 *  @date 2025
 * 
 *  @copyright
 *  Copyright (C) 2025, basyair7
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

// run program if WiFi connecting
void ProgramWiFiClass::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println(F("\nConnection to AP Successfully"));
}

// run program if WiFi connecting
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
    }
}

// got ip address if wifi connected
void ProgramWiFiClass::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    this->LOCALIPServer = WiFi.localIP().toString().c_str();
    Serial.println(F("WiFi Connected"));
    Serial.print(F("IP Address : "));
    Serial.println(this->LOCALIPServer);
}

// run program mode STA
void ProgramWiFiClass::wifi_mode_sta() {
    // Setup WiFi
    // WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_STA);
    this->Bootbtn_obj.begin();

    // Disable sleep mode
    WiFi.setSleep(false);

    // register event handlers
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

    // Attempt to connect to WiFi
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

// run program mode AP
void ProgramWiFiClass::wifi_mode_ap() {
    // Setup WiFi mode AP
    WiFi.mode(WIFI_AP);

    // initializing Access Point
    WiFi.softAP(
        this->__SSID_AP__.c_str(),
        this->__PASS_AP__.c_str()
    );

    // get IP Adrress
    this->LOCALIPServer = WiFi.softAPIP().toString().c_str();
    Serial.print(F("IP Address: "));
    Serial.println(this->LOCALIPServer);
    Serial.println();
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ProgramWiFi)
ProgramWiFiClass ProgramWiFi;
#endif