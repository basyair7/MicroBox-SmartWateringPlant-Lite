/**
 *  @file wifi_sta_config.cpp
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
#include "MicroBox/externobj"

void WebServerClass::WiFi_STA_Config_Main(AsyncWebServerRequest *req) {
    if (WiFi.getMode() == WIFI_STA)
        wifi_sta_config_1(req);
    else
        wifi_sta_config_2(req);
}

void WebServerClass::Save_WiFi_STA_Config(AsyncWebServerRequest *req) {
    // read file html
    String page = this->file_buffer(DIRHTML + "save_config_wifi_sta.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // get input arguments
    String New_SSID, New_Pass;
    if (req->hasArg("newssid") && req->hasArg("newpassword")) {
        New_SSID = req->arg("newssid");
        New_Pass = req->arg("newpassword");
        lfsprog.changeConfigWiFi_STA(New_SSID, New_Pass);
    }

    // get ip address
    clientIP = req->client()->localIP();
    LocalIP  = clientIP.toString();

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%NEW_SSID%",
        "%NEW_PASS%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        New_SSID.c_str(),
        New_Pass.c_str(),
        this->LocalIP.c_str()
    };

    // replace page
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}

void WebServerClass::wifi_sta_config_1(AsyncWebServerRequest *req) {
    // read file html
    String page = this->file_buffer(DIRHTML + "config_wifi_sta_1.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%VERSION_PROJECT%",
        "%SSID_AP%", 
        "%PASS_AP%",
        "%SSID_AP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->__VERSION_PROJECT__.c_str(),
        ProgramWiFi.__SSID_AP__.c_str(),
        ProgramWiFi.__PASS_AP__.c_str(),
        ProgramWiFi.__SSID_AP__.c_str()
    };

    // replace page
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}

void WebServerClass::wifi_sta_config_2(AsyncWebServerRequest *req) {
    // read file html
    String page = file_buffer(DIRHTML + "config_wifi_sta_2.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // get ip address
    clientIP = req->client()->localIP();
    LocalIP = clientIP.toString();

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%VERSION_PROJECT%",
        "%SSID_STA%",
        "%PASS_STA%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->__VERSION_PROJECT__.c_str(),
        ProgramWiFi.__SSID_STA__.c_str(),
        ProgramWiFi.__PASS_STA__.c_str(),
        this->LocalIP.c_str()
    };

    // replace page
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}