/**
 * @file rtc_config.cc
 * @version 1.0.1
 * @author basyair7
 * @date 2024 
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

void WebServer::RTC_Config_Main(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "config_rtc.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    // get ip address
    this->__LOCALIP__ = req->client()->localIP().toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%",
        "%LOCALIP%"
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
}

void WebServer::Save_RTC_Config(AsyncWebServerRequest *req) {
    // read file html
    String page = this->__file_buffer__(this->DIRHTML + "save_config_rtc.html");
    if (page == "") {
        this->__handle_not_found__(req);
        return;
    }

    int __MONTH__, __DAY__, __YEAR__;
    int __HOUR__, __MINUTE__, __SECOND__;

    if (req->hasArg("date") && req->hasArg("time") && req->hasArg("seconds")) {
        String date = req->arg("date");   // Format: YYYY-MM-DD
        String time = req->arg("time");   // Format: HH:MM
        
        // Check 12-hour format
        rtcprog.twelve_hour_format = req->hasArg("twelve_hour_format");

        // Parse date
        __YEAR__  = date.substring(0, 4).toInt();
        __MONTH__ = date.substring(5, 7).toInt();
        __DAY__   = date.substring(8, 10).toInt();

        // Parse time
        __HOUR__   = time.substring(0, 2).toInt();
        __MINUTE__ = time.substring(3, 5).toInt();
        __SECOND__ = req->arg("seconds").toInt();

        rtcprog.manualAdjust(
            __MONTH__, __DAY__, __YEAR__,
            __HOUR__, __MINUTE__, __SECOND__
        );
    }

    // get ip address
    this->__LOCALIP__ = req->client()->localIP().toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->__LOCALIP__.c_str()
    };

    // replace page
    for (int i = 0; i < (int)(sizeof(tags_html) / sizeof(tags_html[0])); i++) {
        page.replace(placeholders[i], tags_html[i]);
    }

    req->send_P(200, TEXTHTML, page.c_str());
}
