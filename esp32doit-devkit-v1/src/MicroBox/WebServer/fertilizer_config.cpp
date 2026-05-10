/**
 *  @file fertilizer_config.cpp
 *  @version 1.0.1
 *  @brief 肥料設定関連のWebサーバー関数ファイル。
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

void WebServerClass::Fertilizer_Config_Main(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(this->DIRHTML + "config_fertilizer.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    // IPアドレスを取得
    this->LocalIP = req->client()->localIP().toString();

    String _time_l = String(fertilizerProg.__HOUR_FERTILIZER__) + ":" +
                    String(fertilizerProg.__MINUTE_FERTILIZER__) + ":" +
                    String(fertilizerProg.__SECOND_FERTILIZER__);
    
    String _interval_l = String(fertilizerProg.__INTERVAL_FERTILIZER__);

    const String placeholders[] = {
        "%VERSIONPROJECT%",
        "%TIME%",
        "%INTERVAL%",

        "%PPM_TARGET%",
        "%PPM_TOLERANCE%",
        "%LOCALIP%"
    };

    const String tags_html[] = {
        this->__VERSION_PROJECT__,
        _time_l,
        _interval_l,
        String(lfsprog.__PPM_TARGET__),
        String(lfsprog.__PPM_TOLERANCE__),
        this->LocalIP
    };

    // ページを置き換え
    for (size_t i = 0; i < sizeof(tags_html)/sizeof(tags_html[0]); i++)
        page.replace(placeholders[i], tags_html[i]);
    
    req->send(200, TEXTHTML, page);
}

void WebServerClass::Save_Fertilizer_Config(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(this->DIRHTML + "save_config_fertilizer.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    int __ppm_target, __ppm_tolerance;
    if (req->hasArg("ppm_target") && req->hasArg("ppm_tolerance")) {
        String ppm_target    = req->arg("ppm_target");
        String ppm_tolerance = req->arg("ppm_tolerance");

        __ppm_target    = ppm_target.toInt();
        __ppm_tolerance = ppm_tolerance.toInt();

        fertilizerProg.__PPM_TARGET__    = __ppm_target;
        fertilizerProg.__PPM_TOLERANCE__ = __ppm_tolerance;
        
        lfsprog.changeFertilizerConfig(__ppm_target, __ppm_tolerance);
    }

    // 成功レスポンスを返す
    StaticJsonDocument<200> response;
    response["status"] = "success";
    response["message"] = "Fertilizer configuration saved successfully.";
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    req->send_P(200, APPJSON, jsonResponse.c_str());
}

void WebServerClass::Save_Fertilizer_ConfigTime(AsyncWebServerRequest *req) {
    // HTMLファイルを読み込む
    String page = this->file_buffer(this->DIRHTML + "save_config_fertilizer.html");
    if (page == "") {
        this->handleNotFound(req);
        return;
    }

    uint8_t  __HOUR__, __MINUTE__;
    uint32_t __SECOND__, __INTERVAL__;
    // クエリパラメータから肥料設定を取得
    if (req->hasArg("time") && req->hasArg("second") && req->hasArg("interval")) {
        String time     = req->arg("time");
        String second   = req->arg("second");
        String interval = req->arg("interval");

        //　時間を分割して取得
        __HOUR__ = time.substring(0, 2).toInt();
        __MINUTE__ = time.substring(3, 5).toInt();
        __SECOND__ = second.toInt();
        __INTERVAL__ = interval.toInt();

        fertilizerProg.__HOUR_FERTILIZER__     = __HOUR__;
        fertilizerProg.__MINUTE_FERTILIZER__   = __MINUTE__;
        fertilizerProg.__SECOND_FERTILIZER__   = __SECOND__;
        fertilizerProg.__INTERVAL_FERTILIZER__ = __INTERVAL__;

        lfsprog.changeFertilizerConfigTime(
            __HOUR__, __MINUTE__, __SECOND__, __INTERVAL__
        );
    }

    // 成功レスポンスを返す
    StaticJsonDocument<200> response;
    response["status"] = "success";
    response["message"] = "Fertilizer configuration time saved successfully.";
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    req->send_P(200, APPJSON, jsonResponse.c_str());
}