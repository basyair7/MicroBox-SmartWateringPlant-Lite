/**　
 *  @file Routes.cpp
 *  @version 1.0.0
 *  @date 2025
 *  @author basyair7
 *  
 *  @copyright
 *  Copyright (C) 2025, basyair7
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "MicroBox/software/WebServer"

void WebServerClass::RoutesSystem() {
    // If page not found
    this->serverAsync.onNotFound(
        std::bind(
            &WebServerClass::handleNotFound, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint for index page
    this->serverAsync.on("/", HTTP_GET,
        std::bind(
            &WebServerClass::index, this,
            std::placeholders::_1
        )
    );

    this->serverAsync.on("/index", HTTP_GET,
        std::bind(
            &WebServerClass::index, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP Get endpoint for rest-api data-server
    this->serverAsync.on("/data-server", HTTP_GET, 
        std::bind(
            &WebServerClass::DataWebServer, this,
            std::placeholders::_1
        )
    );
    
    // Setup HTTP GET endpoint for recovery page
    this->serverAsync.on("/recovery", HTTP_GET, 
        std::bind(
            &WebServerClass::RecoveryPage, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint for enable blynk
    this->serverAsync.on("/enable-blynk", HTTP_GET,
        std::bind(
            &WebServerClass::EnableBlynk, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to reboot the system
    this->serverAsync.on("/rst-webserver", HTTP_GET,
        std::bind(
            &WebServerClass::RebootSys, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to reset system
    this->serverAsync.on("/reset-system", HTTP_GET,
        std::bind(
            &WebServerClass::ResetSys, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to config wifi sta
    this->serverAsync.on("/config-wifi-sta", HTTP_GET,
        std::bind(
            &WebServerClass::WiFi_STA_Config_Main, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to save config wifi sta
    this->serverAsync.on("/save-config-wifi-sta", HTTP_POST,
        std::bind(
            &WebServerClass::Save_WiFi_STA_Config, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to config wifi ap
    this->serverAsync.on("/config-wifi-ap", HTTP_GET,
        std::bind(
            &WebServerClass::WiFi_AP_Config_Main, this,
            std::placeholders::_1
        )
    );

    // Setup HTTP GET endpoint to save config wifi ap
    this->serverAsync.on("/save-config-wifi-ap", HTTP_POST,
        std::bind(
            &WebServerClass::Save_WiFi_AP_Config, this,
            std::placeholders::_1
        )
    );

    // Update auto change state WiFi mode
    this->serverAsync.on("/auto-change-wifi-mode", HTTP_GET,
        std::bind(
            &WebServerClass::UpdateAutoChangeWiFi, this,
            std::placeholders::_1
        )
    );
}

void WebServerClass::RoutesRelay() {
    // update relay state (post method)
    this->serverAsync.on("/post-relay", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
        std::bind(
            &WebServerClass::postRelay, this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            std::placeholders::_5
        )
    );

    // query data relay (return format json)
    this->serverAsync.on("/query-relay", HTTP_GET,
        std::bind(
            &WebServerClass::queryDataRelay, this, std::placeholders::_1
        )
    );

    // getRelayState
    this->serverAsync.on("/getRelayStatus", HTTP_GET,
        std::bind(
            &WebServerClass::readRelayState, this,
            std::placeholders::_1
        )
    );

    // update relay state (get method)
    this->serverAsync.on("/check", HTTP_GET,
        std::bind(
            &WebServerClass::checkRelayState, this, std::placeholders::_1
        )
    );

    // update auto watering
    this->serverAsync.on("/auto-watering", HTTP_GET,
        std::bind(
            &WebServerClass::AutoWatering, this, std::placeholders::_1
        )
    );

    // update manual watering
    this->serverAsync.on("/manual-watering", HTTP_GET,
        std::bind(
            &WebServerClass::ManualWatering, this, std::placeholders::_1
        )
    );
}

void WebServerClass::Routes() {
    RoutesSystem();
    RoutesRelay();
}