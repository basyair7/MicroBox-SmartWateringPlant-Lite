/**
 *  @file reset_sys.cpp
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

void WebServerClass::ResetSys(AsyncWebServerRequest *req) {
    // read file html
    String page = this->file_buffer(DIRHTML + "reset-sys.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // get ip address
    clientIP = req->client()->localIP();
    LocalIP = clientIP.toString();

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->LocalIP.c_str()
    };

    // replace page
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send_P(200, TEXTHTML, page.c_str());

    // reinitailizing config
    lfsprog.reinitializeState();
    lfsprog.reinitializeVarRelay();
    lfsprog.reinitializeWiFiConfig();
}