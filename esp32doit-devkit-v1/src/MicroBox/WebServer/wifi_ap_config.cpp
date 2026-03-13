/**
 *  @file wifi_ap_config.cpp
 *  @version 1.0.1
 *  @brief WiFi AP設定を扱うWebサーバー関数ファイル。
 *  @date 2026
 *  @author basyair7
 *  
 *  @copyright
 *  Copyright (C) 2026, basyair7
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

/**
 * @brief WiFi AP設定のメイン画面を表示します。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::WiFi_AP_Config_Main(AsyncWebServerRequest *req) {
    if (WiFi.getMode() == WIFI_STA) 
        wifi_ap_config_1(req);
    else
        wifi_ap_config_2(req);
}

/**
 * @brief WiFi AP設定を保存します。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::Save_WiFi_AP_Config(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(DIRHTML + "save_config_wifi_ap.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // 入力引数を取得
    String New_SSID, New_Pass;
    if (req->hasArg("newssid") && req->hasArg("newpassword")) {
        New_SSID = req->arg("newssid");
        New_Pass = req->arg("newpassword");
        lfsprog.changeConfigWiFi_AP(New_SSID, New_Pass);
    }

    // IPアドレスを取得
    clientIP = req->client()->localIP();
    LocalIP = clientIP.toString();

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

    // ページを置き換え
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}

/**
 * @brief WiFi AP設定画面1を表示します（STAモード時）。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::wifi_ap_config_1(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(DIRHTML + "config_wifi_ap_1.html");
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

    // ページを置き換え
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html); i++)
        page.replace(placeholders[i], tags_html[i]);

    req->send(200, TEXTHTML, page);
}

/**
 * @brief WiFi AP設定画面2を表示します（APモード時）。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::wifi_ap_config_2(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = file_buffer(DIRHTML + "config_wifi_ap_2.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // IPアドレスを取得
    clientIP = req->client()->localIP();
    LocalIP = clientIP.toString();

    const char *placeholders[] = {
        "%VERSION_PROJECT%",
        "%VERSION_PROJECT%",
        "%SSID_AP%",
        "%PASS_AP%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->__VERSION_PROJECT__.c_str(),
        ProgramWiFi.__SSID_AP__.c_str(),
        ProgramWiFi.__PASS_AP__.c_str(),
        this->LocalIP.c_str()
    };

    // ページを置き換え
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);
    
    req->send(200, TEXTHTML, page);
}