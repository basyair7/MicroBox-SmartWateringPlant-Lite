/**
 *  @file index.cpp
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
#include "MicroBox/hardware/RelayController"
#include "MicroBox/externobj"

void WebServerClass::index(AsyncWebServerRequest *req) {
    // Read file html
    String page = this->file_buffer(this->DIRHTML + "index.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // get list data relay
    std::vector<String>listVar;
    std::vector<int>listId, listPin;

    for (const auto &relay : RELAY_PINS) {
        relayController.read(String(VAR_SWITCH) + String(relay));
        listPin.push_back(relayController.pins_io_relay);
        listVar.push_back(relayController.label_relay);
        listId.push_back(relayController.id_relay);
    }

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%VAR1%", "%VARID1%", "%CHECKED1%",
        "%VAR2%", "%VARID2%", "%CHECKED2%",
        "%VAR3%", "%VARID3%", "%CHECKED3%"
    };

    const char *tags_html[] = {
        (this->__VERSION_PROJECT__).c_str(),
        "Auto Watering", "auto", this->stateChecked(wateringSys.AutoWateringState).c_str(),
        (listVar[0]).c_str(), String(listPin[0]).c_str(), this->RelayChecked(listPin[0]).c_str(),
        listVar[1].c_str(), String(listPin[1]).c_str(), this->RelayChecked(listPin[1]).c_str()
    };

    // Replace page
    for (size_t item = 0; item < sizeof(tags_html)/sizeof(tags_html[0]); item++) {
        page.replace(placeholders[item], tags_html[item]);
    }

    req->send(200, TEXTHTML, page);
}