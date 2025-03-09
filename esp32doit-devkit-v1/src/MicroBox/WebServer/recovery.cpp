/**
 *  @file recovery.cpp
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

void WebServerClass::RecoveryPage(AsyncWebServerRequest *req) {
    // read file html
    String page = this->file_buffer(this->DIRHTML + "recovery.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // get ip address
    this->LocalIP = req->client()->localIP().toString();

    // convert port typedata to char
    char portWeb[10 + sizeof(char)];
    sprintf(portWeb, "%d", this->port);

    // replace localip
    this->LocalIP = this->LocalIP + ":" + portWeb;

    // read auto change wifi state
    bool _autoChangeWiFi;
    lfsprog.readConfigState(AUTOCHANGE, &_autoChangeWiFi);

    const String placeholders[] = {
        "%VERSION_PROJECT%", 
        "%HW_VERSION%", 
        "%SW_VERSION%",
        "%BUILD_DATE%", 
        "%FIRMWARE_REGION%",
        "%LOCALIP%", 
        "%PORT%",
        
        "%SSID_AP%", 
        "%PASS_AP%",
        "%SSID_STA", 
        "%PASS_STA%",
        "%AUTO_CHANGE_WIFI%",
        
        "%LOCALIP%", "%LOCALIP%", "%LOCALIP%"
    };

    const String tags_html[] = {
        this->__VERSION_PROJECT__,
        this->__HW_VERSION__,
        this->__SW_VERSION__,
        this->__BUILD_DATE__,
        this->__REGION__,
        this->LocalIP,
        portWeb,

        ProgramWiFi.__SSID_AP__,
        ProgramWiFi.__PASS_AP__,
        ProgramWiFi.__SSID_STA__,
        ProgramWiFi.__PASS_STA__,
        _autoChangeWiFi ? "Enable" : "Disable",

        this->LocalIP, this->LocalIP, this->LocalIP
    };

    // replace page
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}