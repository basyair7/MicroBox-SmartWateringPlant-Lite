/**
 *  @file wifi_ap_config.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

void WebServer::WiFi_AP_Config_Main(AsyncWebServerRequest *req) {
    if (WiFi.getMode() == WIFI_STA) 
        this->__wifi_ap_config_1__(req);
    else
        this->__wifi_ap_config_2__(req);
}

void WebServer::Save_WiFi_AP_Config(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "save_config_wifi_ap.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get input arguments
    String __NEW_SSID_AP__, __NEW_PSK_AP__;
    if (req->hasArg("newssidap") && req->hasArg("newpasswordap")) {
        __NEW_SSID_AP__ = req->arg("newssidap");
        __NEW_PSK_AP__  = req->arg("newpasswordap");
        lfsprog.changeConfigWiFi_AP(__NEW_SSID_AP__, __NEW_PSK_AP__);
    }

    // get ip address
    IPAddress clientIP =  req->client()->localIP();
    this->__LOCALIP__ = clientIP.toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%", "%NEWSSIDAP%", "%NEWPASSWORDAP%", "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        __NEW_SSID_AP__.c_str(),
        __NEW_PSK_AP__.c_str(),
        this->__LOCALIP__.c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send(200, TEXTHTML, page);
}

void WebServer::__wifi_ap_config_1__(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "config_wifi_ap_1.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    const char *placeholders[] = {
        "%VERSIONPROJECT%", "%VERSIONPROJECT%", "%SSIDAP%",
        "%PASSWORDAP%", "%SSIDAP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(), this->__VERSION_PROJECT__.c_str(),
        wifiprog.__SSID_AP__.c_str(), wifiprog.__PASSWORD_AP__.c_str(),
        wifiprog.__SSID_AP__.c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send(200, TEXTHTML, page);
}

void WebServer::__wifi_ap_config_2__(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "config_wifi_ap_2.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get ip address
    IPAddress clientIP = req->client()->localIP();
    this->__LOCALIP__ = clientIP.toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%", "%VERSIONPROJECT%",
        "%SSIDAP%", "%PASSWORDAP%", "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(), this->__VERSION_PROJECT__.c_str(),
        wifiprog.__SSID_AP__.c_str(), wifiprog.__PASSWORD_AP__.c_str(),
        this->__LOCALIP__.c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send(200, TEXTHTML, page.c_str());
}
