/**
 *  @file rtc_config.cpp
 *  @version 1.0.1
 *  @brief RTC設定を扱うWebサーバー関数ファイル。
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
 * @brief RTC設定のメイン画面を表示します。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::RTC_Config_Main(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(this->DIRHTML + "config_rtc.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // IPアドレスを取得
    this->LocalIP = req->client()->localIP().toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->LocalIP.c_str()
    };

    // ページを置き換え
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++)
        page.replace(placeholders[i], tags_html[i]);
    
    req->send_P(200, TEXTHTML, page.c_str());
}

/**
 * @brief RTC設定を保存します。
 * @param req 非同期Webサーバーリクエスト
 */
void WebServerClass::Save_RTC_Config(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(this->DIRHTML + "save_config_rtc.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    int __MONTH__, __DAY__, __YEAR__;
    int __HOUR__, __MINUTE__, __SECOND__;

    if (req->hasArg("date") && req->hasArg("time") && req->hasArg("seconds")) {
        String date = req->arg("date"); // フォーマット: YYYY-MM-DD
        String time = req->arg("time"); // フォーマット: HH:MM

        
        // 12時間形式を確認
        rtcprog._twelve_hour_format = req->hasArg("twelve_hour_format");

        // 日付を解析
        __YEAR__  = date.substring(0, 4).toInt();
        __MONTH__ = date.substring(5, 7).toInt();
        __DAY__   = date.substring(8, 10).toInt();

        // 時間を解析
        __HOUR__   = time.substring(0, 2).toInt();
        __MINUTE__ = time.substring(3, 5).toInt();
        __SECOND__ = req->arg("seconds").toInt();

        rtcprog.manualAdjust(
            __YEAR__, __MONTH__, __DAY__,
            __HOUR__, __MINUTE__, __SECOND__
        );
    }

    // IPアドレスを取得
    this->LocalIP = req->client()->localIP().toString();

    const char *placeholders[] = {
        "%VERSIONPROJECT%",
        "%LOCALIP%"
    };

    const char *tags_html[] = {
        this->__VERSION_PROJECT__.c_str(),
        this->LocalIP.c_str()
    };

    // ページを置き換え
    for (int i = 0; i < (int)(sizeof(tags_html)/sizeof(tags_html[0])); i++)
        page.replace(placeholders[i], tags_html[i]);
    
    req->send_P(200, TEXTHTML, page.c_str());
}