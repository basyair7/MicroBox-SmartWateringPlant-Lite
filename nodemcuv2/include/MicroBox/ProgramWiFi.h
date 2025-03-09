/**
 *  @file ProgramWiFi.h
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

#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <functional>
#include "BootButton.h"
#include "variable.h"

class ProgramWiFi {
    public:
        String __SSID_STA__, __PASSWORD_STA__;
        String __SSID_AP__, __PASSWORD_AP__;
        String __LOCALIPServer__;

    public:
        void setup(const String SSID_STA, const String PASSWORD_STA, const String SSID_AP, const String PASSWORD_AP)
        {
            this->__SSID_STA__ = SSID_STA;
            this->__PASSWORD_STA__ = PASSWORD_STA;
            this->__SSID_AP__ = SSID_AP;
            this->__PASSWORD_AP__ = PASSWORD_AP;
        }

        /*! initialize wifi func
        *  @param bool state ? mode AP = true : mode STA = false
        */
        void initWiFi(bool state) {
            Serial.print(F("\nMode WiFi : "));
            Serial.println(state ? F("WIFI_STA") : F("WIFI_AP"));
            if (state) {
                this->__wifi_mode_sta__();
            }
            else {
                this->__wifi_mode_ap__();
            }
        }

        void optimizeWiFi(WiFiSleepType type, bool persistentWiFi) {
            WiFi.setSleepMode(type);
            WiFi.persistent(persistentWiFi);
        }

    private:
        BootButton __bootbtn__ = BootButton(BOOTBUTTON, INPUT);
        WiFiEventHandler __wifiConnect_e_h__, __wifiDisconnect_e_h__;
        void onWiFiConnect(const WiFiEventStationModeGotIP &event);
        void onWiFiDisconnect(const WiFiEventStationModeDisconnected &event);
        void __wifi_mode_ap__(), __wifi_mode_sta__();
};
