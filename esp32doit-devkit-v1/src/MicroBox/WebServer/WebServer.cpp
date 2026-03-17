/**
 *  @file WebServer.cpp
 *  @version 1.0.1
 *  @date 2026
 *  @author basyair7
 *  
 *  @brief このファイルは、WebServerClassの実装を含む。Webサーバーの初期化、ルーティングの設定、および関連するハンドラー関数の実装を提供する。
 *         Webサーバーは、HTTPリクエストを処理し、適切なレスポンスを返すための機能を提供する。これには、HTMLページの提供、APIエンドポイントの処理、およびWebSocket通信の管理が含まれる。
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
#include "MicroBox/software/ProgramWiFi"


void onOTAStart() {
    otaDisplay = true;
    if (!xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) return;
    lcd.clear();
    lcd.print("OTA update started!");    
    xSemaphoreGive(i2cMutex);
}

void onOTAProgress(size_t current, size_t final) {
    static unsigned long ota_progress_millis = 0;

    if (millis() - ota_progress_millis > 500) {
        ota_progress_millis = millis();

        if (!xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) return;

        int progress = (current * 100) / final;
        int bars = progress / 6; // max ~16 char

        lcd.clear();
        lcd.print("OTA Updating", 0, 0);

        lcd.setCursor(0, 1);
        for (int i = 0; i < bars; i++) {
            lcd.print("#");
        }

        xSemaphoreGive(i2cMutex);
    }
}

void onOTAEnd(bool success) {
    if (!xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) return;

    lcd.clear();
    lcd.print(!success ? "OTA update" : "OTA error", 0, 0);
    lcd.print(!success ? "Success!" : "Update failed", 0, 1);

    xSemaphoreGive(i2cMutex);
}

void WebServerClass::ServerInit() {
    // LittleFSを初期化する。
    // lfsprog.setupLFS();

    // ローカルネットワーク探索のために、mDNSレスポンダを初期化する。
    if (!MDNS.begin("esp32-delay")) {
        Serial.println(F("Error starting mDNS"));
        return;
    }

    // OTA (Over The Air) 更新のために、ElegantOTAを初期化する。
    ElegantOTA.begin(&this->serverAsync);
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    // WebSocketをセットアップする。クライアントが接続したときのイベントハンドラーを指定する。
    this->ws.onEvent(std::bind(
        &WebServerClass::onEvent, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6
    ));

    // serverAsyncにWebSocketハンドラを追加する。
    this->serverAsync.addHandler(&this->ws);

    // CSSおよびJavaScriptファイルの配信を実行する。
    this->run_css_js_webserver();
    
    // ルーティングの設定を実行する。
    this->Routes();

    // Webサーバーを開始する。
    this->serverAsync.begin();
    Serial.println(F("HTTP Started..."));
    this->LocalIP = ProgramWiFi.LOCALIPServer;
    Serial.printf("http://%s:%d/index\t-> index page\n",
        this->LocalIP.c_str(), this->port);
    Serial.printf("http://%s:%d/recovery\t-> recovery page\n",
        this->LocalIP.c_str(), this->port);
}

void WebServerClass::UpdateOTAloop() {
    // OTA更新のループを実行する。これにより、OTA更新が適切に処理されるようになる。
    ElegantOTA.loop();
}

void WebServerClass::run_css_js_webserver() {
    // CSSファイルとJavaScriptファイルのリストを定義する。
    const std::vector<String> list_css_files = {
        "recovery.css", "index.css", 
        "config_wifi_ap.css", "config_wifi_sta.css",
        "config_rtc.css"
    };

    const std::vector<String> list_js_files = {
        "clock.js", "data_server.js", "reboot.js",
        "reset-sys.js", "sweetalert.min.js", "switchBlynk.js",
        "toggleCheck.js", "date_time_rtc.js"
    };

    // 各CSSファイルを提供するための静的ルートを設定する。
    for (const auto fileName : list_css_files)
        this->serverAsync.serveStatic(
            ("/css/" + fileName).c_str(), LFS,
            (DIRCSS + fileName).c_str()
        );
    
    // 各JavaScriptファイルを提供するための静的ルートを設定する。
    for (const auto fileName : list_js_files)
        this->serverAsync.serveStatic(
            ("/js/" + fileName).c_str(), LFS,
            (DIRJS + fileName).c_str()
        );
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_WebServer)
/**
 * @brief WebServerClassのグローバルインスタンスを定義する。これにより、他のファイルからWebServerオブジェクトを直接使用できるようになる。
 * このインスタンスは、Webサーバーの初期化とルーティングの設定を行うために使用される。
 * 注意: NO_GLOBAL_INSTANCESまたはNO_GLOBAL_WebServerが定義されている場合、このインスタンスは定義されないため、他のファイルからWebServerオブジェクトを使用することはできなくなる。
 */
WebServerClass WebServer;
#endif