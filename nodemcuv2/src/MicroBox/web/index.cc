/**
 *  @file index.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"
#include "MicroBox/RelayController.h"

void WebServer::index(AsyncWebServerRequest *req) {
    // read file html
    String _page = this->__file_buffer__(this->DIRHTML + "index.html");
    if (_page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get ip address
    // this->__LOCALIP__ = req->client()->localIP().toString();

    // get list data relay
    std::vector<String>listVar;
    std::vector<int>listId, listPin;

    for (const auto &relay : RELAY_PINS) {
        RelayController::read(PIN_IO_ALIAS[relay]);
        listPin.push_back(RelayController::PIN_IO_RELAY);
        listVar.push_back(RelayController::LABEL_RELAY);
        listId.push_back(RelayController::ID_RELAY);
    }

    const char *placeholders[] = {
        "%VERSIONPROJECT%",
        "%VAR1%", "%VARID1%", "%CHECKED1%",
        "%VAR2%", "%VARID2%", "%CHECKED2%",
        "%VAR3%", "%VARID3%", "%CHECKED3%"
    };

    const char *tags_html[] = {
        (this->__VERSION_PROJECT__).c_str(),
        "Auto Watering", "auto", this->StateChecked(wateringsys.AutoWateringState).c_str(),
        (listVar[0]).c_str(), (PIN_IO_ALIAS[listPin[0]]).c_str(), (this->RelayChecked(listPin[0])).c_str(),
        (listVar[1]).c_str(), (PIN_IO_ALIAS[listPin[1]]).c_str(), (this->RelayChecked(listPin[1])).c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        _page.replace(placeholders[i], tags_html[i]);
    }

    req->send_P(200, TEXTHTML, _page.c_str());
}
