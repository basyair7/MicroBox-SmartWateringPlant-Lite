/**
 *  @file recovery.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

void WebServer::RecoveryPage(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "recovery.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get ip address
    this->__LOCALIP__ = req->client()->localIP().toString().c_str();
    
    // convert port typedata to char
    char portWeb[10 + sizeof(char)];
    sprintf(portWeb, "%d", this->__PORT__);

    // replace localIP
    String localIP_replace = this->__LOCALIP__ + ":" + portWeb;

    // read auto change wifi state
    bool __autoChangeWiFi;
    lfsprog.readConfigState(AUTOCHANGE, &__autoChangeWiFi);


    const String placeholders[] = {
        "%VERSIONPROJECT%", "%HWVERSION%", "%SWVERSION%",
        "%BUILDDATE%", "%FIRMWAREREGION%",
        "%LOCALIP%", "%PORT%",
        "%SSIDAP%", "%PASSWORDAP%", "%SSIDSTA%", "%PASSWORDSTA%", "%AUTOCHANGEWIFI%",
        "%LOCALIP%", "%LOCALIP%", "%LOCALIP%"
    };

    const String tags_html[] = {
        this->__VERSION_PROJECT__,
        this->__HW_VERSION__,
        this->__SW_VERSION__,
        this->__BUILD_DATE__,
        this->__REGION__,
        this->__LOCALIP__,
        portWeb,

        wifiprog.__SSID_AP__,
        wifiprog.__PASSWORD_AP__,
        wifiprog.__SSID_STA__,
        wifiprog.__PASSWORD_STA__,
        __autoChangeWiFi ? "Enable" : "Disable",

        localIP_replace, localIP_replace, localIP_replace, localIP_replace
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send(200, TEXTHTML, page);
}
