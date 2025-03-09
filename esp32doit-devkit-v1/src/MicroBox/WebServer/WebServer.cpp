/**
 *  @file WebServer.cpp
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
#include "MicroBox/software/ProgramWiFi"

void WebServerClass::ServerInit() {
    // Initialize LittleFS
    lfsprog.setupLFS();

    // Initialize mDNS with the hostname esp32-delay
    if (!MDNS.begin("esp32-delay")) {
        Serial.println(F("Error starting mDNS"));
        return;
    }

    // Initialize ElegantOTA for over-the-air update
    ElegantOTA.begin(&this->serverAsync);

    // setup WebSocket
    this->ws.onEvent(std::bind(
        &WebServerClass::onEvent, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6
    ));

    // add Handler WebSocket on serverAsync
    this->serverAsync.addHandler(&this->ws);

    // Serve CSS and JavaScript file
    this->run_css_js_webserver();
    
    // Run Routes program
    this->Routes();

    // initializes AsyncWebServer
    this->serverAsync.begin();
    Serial.println(F("HTTP Started..."));
    this->LocalIP = ProgramWiFi.LOCALIPServer;
    Serial.printf("http://%s:%d/index\t-> index page\n",
        this->LocalIP.c_str(), this->port);
    Serial.printf("http://%s:%d/recovery\t-> recovery page\n",
        this->LocalIP.c_str(), this->port);
}

void WebServerClass::UpdateOTAloop() {
    // Handle OTA updates in the loop
    ElegantOTA.loop();
}

void WebServerClass::run_css_js_webserver() {
    // Define lists of CSS and JavaScript files to serve
    const std::vector<String> list_css_files = {
        "recovery.css", "index.css", 
        "config_wifi_ap.css", "config_wifi_sta.css"
    };

    const std::vector<String> list_js_files = {
        "clock.js", "data_server.js", "reboot.js",
        "reset-sys.js", "sweetalert.min.js", "switchBlynk.js",
        "toggleCheck.js"
    };

    // Serve each CSS file
    for (const auto fileName : list_css_files)
        this->serverAsync.serveStatic(
            ("/css/" + fileName).c_str(), LFS,
            (DIRCSS + fileName).c_str()
        );
    
    // Serve each JavaScript file
    for (const auto fileName : list_js_files)
        this->serverAsync.serveStatic(
            ("/js/" + fileName).c_str(), LFS,
            (DIRJS + fileName).c_str()
        );
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WebServer)
/**
 * Create a global instance of WebServerClass
 */
WebServerClass WebServer;
#endif