/**
 *  @file WebServer.cc
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

#include "MicroBox/WebServer.h"
#include "MicroBox/variable.h"
#include "MicroBox/externprog.h"

void WebServer::ServerInit() {
    if (MDNS.begin("esp8266")) {
        Serial.println(F("MDNS response started"));
    }

    // initializes ElegantOTA
    ElegantOTA.begin(&this->serverAsync);

    // setup WebSocket
    this->ws.onEvent(std::bind(
        &WebServer::onEvent, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6
    ));

    // add Handler WebSocket on serverAsync
    this->serverAsync.addHandler(&this->ws);

    this->__run_css_js_webserver__();

    // if page not found
    this->serverAsync.onNotFound(
        std::bind(
            &WebServer::__handle_not_found__, this, std::placeholders::_1
        )
    );

    // index page
    this->serverAsync.on("/", HTTP_GET,
        std::bind(
            &WebServer::index, this, std::placeholders::_1
        )
    );
    this->serverAsync.on("/index", HTTP_GET,
        std::bind(
            &WebServer::index, this, std::placeholders::_1
        )
    );

    // send datetime rtc module to webserver
    this->serverAsync.on("/datetime", HTTP_GET,
        std::bind(
            &WebServer::RTCServer, this, std::placeholders::_1
        )
    );

    // send data to webserver
    this->serverAsync.on("/data-server", HTTP_GET,
        std::bind(
            &WebServer::DataWebServer, this, std::placeholders::_1
        )
    );

    // run recovery
    this->serverAsync.on("/recovery", HTTP_GET,
        std::bind(
            &WebServer::RecoveryPage, this, std::placeholders::_1
        )
    );

    // EnableBlynk
    this->serverAsync.on("/enable-blynk", HTTP_GET,
        std::bind(
            &WebServer::EnableBlynk, this, std::placeholders::_1
        )
    );

    // reboot system
    this->serverAsync.on("/rst-webserver", HTTP_GET,
        std::bind(
            &WebServer::RebootSys, this, std::placeholders::_1
        )
    );

    // reset system
    this->serverAsync.on("/reset-system", HTTP_GET,
        std::bind(
            &WebServer::ResetSys, this, std::placeholders::_1
        )
    );

    // update relay state (post method)
    this->serverAsync.on("/post-relay", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
        std::bind(
            &WebServer::postRelay, this,
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
            &WebServer::queryDataRelay, this, std::placeholders::_1
        )
    );

    // getRelayStatus
    this->serverAsync.on("/getRelayStatus", HTTP_GET,
        std::bind(
            &WebServer::readRelayState, this, std::placeholders::_1
        )
    );

    // update relay state (get method)
    this->serverAsync.on("/check", HTTP_GET,
        std::bind(
            &WebServer::checkRelayState, this, std::placeholders::_1
        )
    );

    // update auto watering
    this->serverAsync.on("/auto-watering", HTTP_GET,
        std::bind(
            &WebServer::AutoWatering, this, std::placeholders::_1
        )
    );

    // update manual watering
    this->serverAsync.on("/manual-watering", HTTP_GET,
        std::bind(
            &WebServer::ManualWatering, this, std::placeholders::_1
        )
    );

    // update auto change state wifi mode
    this->serverAsync.on("/auto-change-wifi-mode", HTTP_GET,
        std::bind(
            &WebServer::updateAutoChangeWiFi, this, std::placeholders::_1
        )
    );

    // config WiFi AP
    this->serverAsync.on("/config-wifi-ap", HTTP_GET,
        std::bind(
            &WebServer::WiFi_AP_Config_Main, this, std::placeholders::_1
        )
    );
    this->serverAsync.on("/save-config-wifi-ap", HTTP_POST,
        std::bind(
            &WebServer::Save_WiFi_AP_Config, this, std::placeholders::_1
        )
    );

    // config WiFi STA
    this->serverAsync.on("/config-wifi-sta", HTTP_GET,
        std::bind(
            &WebServer::WiFi_STA_Config_Main, this, std::placeholders::_1
        )
    );
    this->serverAsync.on("/save-config-wifi-sta", HTTP_POST, 
        std::bind(
            &WebServer::Save_WiFi_STA_Config, this, std::placeholders::_1
        )
    );

    // config rtc
    this->serverAsync.on("/config-rtc", HTTP_GET,
        std::bind(
            &WebServer::RTC_Config_Main, this, std::placeholders::_1
        )
    );
    this->serverAsync.on("/save-config-rtc", HTTP_POST,
        std::bind(
            &WebServer::Save_RTC_Config, this, std::placeholders::_1
        )
    );

    this->serverAsync.begin();
    Serial.println(F("HTTP Started..."));
    this->__LOCALIP__ = wifiprog.__LOCALIPServer__;
    Serial.printf("http://%s:%d/index\t-> index page\n", this->__LOCALIP__.c_str(), this->__PORT__);
    Serial.printf("http://%s:%d/recovery\t-> recovery page\n", this->__LOCALIP__.c_str(), this->__PORT__);
}

void WebServer::updateOTAloop() {
    ElegantOTA.loop();
}

void WebServer::__run_css_js_webserver__(void) {
    // get css file and javascripts file
    // css
    const std::vector<String> list_files_css = {
        "index.css", "recovery.css",
        "config_wifi_ap.css", "config_wifi_sta.css",
        "config_rtc.css"
    };

    // js
    const std::vector<String> list_files_js = {
        "clock.js", "data_server.js", "reboot.js", "reset-sys.js", 
        "sweetalert.min.js", "switchBlynk.js", "date_time_rtc.js",
        "toggleCheck.js"
    };

    // run css files
    for (const auto fileName : list_files_css) {
        this->serverAsync.serveStatic(("/css/" + fileName).c_str(), LFS, (this->DIRCSS + fileName).c_str());
    }

    // run js files
    for (const auto fileName : list_files_js) {
        this->serverAsync.serveStatic(("/js/" + fileName).c_str(), LFS, (this->DIRJS + fileName).c_str());
    }
}
