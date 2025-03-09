/**
 *  @file reset_sys.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

void WebServer::ResetSys(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "reset-sys.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get ip address
    this->__LOCALIP__ = req->client()->localIP().toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%", "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->__LOCALIP__.c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send_P(200, TEXTHTML, page.c_str());

    // reinitializing config
    lfsprog.reinitializeState();
    lfsprog.reinitializeVarRelay();
    lfsprog.reinitializeWiFiConfig();
}
