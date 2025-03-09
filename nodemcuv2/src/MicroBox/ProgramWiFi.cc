/**
 *  @file ProgramWiFi.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
 * 
 *  @copyright
 *  Copyright (C) 2024, basyair7
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

#include "MicroBox/ProgramWiFi.h"
#include "MicroBox/SysHandlers.h"

// run program if WiFi connecting
void ProgramWiFi::onWiFiConnect(const WiFiEventStationModeGotIP &event) {
    Serial.println();
    Serial.print(F("Connected to : ")); Serial.println(this->__SSID_STA__);
    Serial.print(F("IP Address : ")); Serial.println(event.ip);
    this->__LOCALIPServer__ = event.ip.toString().c_str();
}

// run program if WiFi disconnect
void ProgramWiFi::onWiFiDisconnect(const WiFiEventStationModeDisconnected &event) {
    WiFi.disconnect();
    WiFi.begin(this->__SSID_STA__.c_str(), this->__PASSWORD_STA__.c_str());
}

// run program mode STA
void ProgramWiFi::__wifi_mode_sta__() {
    // setup Wifi
    // WiFi.mode(WIFI_AP_STA);
    __bootbtn__.begin();
    WiFi.mode(WIFI_STA);

    // register event handlers
    this->__wifiConnect_e_h__ = WiFi.onStationModeGotIP(std::bind(&ProgramWiFi::onWiFiConnect, this, std::placeholders::_1));
    this->__wifiDisconnect_e_h__ = WiFi.onStationModeDisconnected(std::bind(&ProgramWiFi::onWiFiDisconnect, this, std::placeholders::_1));

    // initializing WiFi and Connecting to an SSID
    WiFi.begin(this->__SSID_STA__.c_str(), this->__PASSWORD_STA__.c_str());
    Serial.println(F("Connecting to WiFi..."));
    while (WiFi.status() != WL_CONNECTED) {
        __bootbtn__.ChangeWiFiMode();
        AutoChangeState::run();
        delay(500);
    }

    // RSSI
    Serial.print(F("\nRRSI : "));
    Serial.println(WiFi.RSSI());
}

// run program mode AP
void ProgramWiFi::__wifi_mode_ap__() {
    // setup wifi mode AP
    WiFi.mode(WIFI_AP);

    // intializing Access Point
    WiFi.softAP(this->__SSID_AP__.c_str(), this->__PASSWORD_AP__.c_str());

    // get IP Address
    this->__LOCALIPServer__ = WiFi.softAPIP().toString().c_str();
    Serial.print(F("IP Address : "));
    Serial.println(this->__LOCALIPServer__);
    Serial.println();
}
