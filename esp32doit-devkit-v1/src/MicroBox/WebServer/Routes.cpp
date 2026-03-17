/**
 *  @file Routes.cpp
 *  @version 1.0.1
 *  @brief Webサーバーのルート設定関連関数ファイル。
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

/**
 * @brief Webサーバーのシステムルートを設定します。
 */
void WebServerClass::RoutesSystem() {
    // ページが見つからない場合
    this->serverAsync.onNotFound(
        std::bind(
            &WebServerClass::handleNotFound, this,
            std::placeholders::_1
        )
    );

    // RTCモジュールの日時をWebサーバーへ送信する。
    this->serverAsync.on("/datetime", HTTP_GET, 
        std::bind(
            &WebServerClass::RTCServer, this,
            std::placeholders::_1
        )
    );

    // インデックスページのHTTP GETエンドポイントを設定
    this->serverAsync.on("/", HTTP_GET,
        std::bind(
            &WebServerClass::index, this,
            std::placeholders::_1
        )
    );

    this->serverAsync.on("/index", HTTP_GET,
        std::bind(
            &WebServerClass::index, this,
            std::placeholders::_1
        )
    );

    // REST-APIデータサーバーのHTTP GETエンドポイントを設定
    this->serverAsync.on("/data-server", HTTP_GET, 
        std::bind(
            &WebServerClass::DataWebServer, this,
            std::placeholders::_1
        )
    );
    
    // リカバリーページのHTTP GETエンドポイントを設定
    this->serverAsync.on("/recovery", HTTP_GET, 
        std::bind(
            &WebServerClass::RecoveryPage, this,
            std::placeholders::_1
        )
    );

    // Blynk有効化のHTTP GETエンドポイントを設定
    this->serverAsync.on("/enable-blynk", HTTP_GET,
        std::bind(
            &WebServerClass::EnableBlynk, this,
            std::placeholders::_1
        )
    );

    // システム再起動のHTTP GETエンドポイントを設定
    this->serverAsync.on("/rst-webserver", HTTP_GET,
        std::bind(
            &WebServerClass::RebootSys, this,
            std::placeholders::_1
        )
    );

    // システムリセットのHTTP GETエンドポイントを設定
    this->serverAsync.on("/reset-system", HTTP_GET,
        std::bind(
            &WebServerClass::ResetSys, this,
            std::placeholders::_1
        )
    );

    // WiFi STA設定のHTTP GETエンドポイントを設定
    this->serverAsync.on("/config-wifi-sta", HTTP_GET,
        std::bind(
            &WebServerClass::WiFi_STA_Config_Main, this,
            std::placeholders::_1
        )
    );

    // WiFi STA設定保存のHTTP POSTエンドポイントを設定
    this->serverAsync.on("/save-config-wifi-sta", HTTP_POST,
        std::bind(
            &WebServerClass::Save_WiFi_STA_Config, this,
            std::placeholders::_1
        )
    );

    // WiFi AP設定のHTTP GETエンドポイントを設定
    this->serverAsync.on("/config-wifi-ap", HTTP_GET,
        std::bind(
            &WebServerClass::WiFi_AP_Config_Main, this,
            std::placeholders::_1
        )
    );

    // WiFi AP設定保存のHTTP POSTエンドポイントを設定
    this->serverAsync.on("/save-config-wifi-ap", HTTP_POST,
        std::bind(
            &WebServerClass::Save_WiFi_AP_Config, this,
            std::placeholders::_1
        )
    );

    // WiFiモード自動変更状態を更新
    this->serverAsync.on("/auto-change-wifi-mode", HTTP_GET,
        std::bind(
            &WebServerClass::UpdateAutoChangeWiFi, this,
            std::placeholders::_1
        )
    );

    // RTC設定のHTTP GETエンドポイントを設定
    this->serverAsync.on("/config-rtc", HTTP_GET,
        std::bind(
            &WebServerClass::RTC_Config_Main, this,
            std::placeholders::_1
        )
    );

    // RTC設定保存のHTTP POSTエンドポイントを設定
    this->serverAsync.on("/save-config-rtc", HTTP_POST,
        std::bind(
            &WebServerClass::Save_RTC_Config, this,
            std::placeholders::_1
        )
    );
}

/**
 * @brief Webサーバーのリレールートを設定します。
 */
void WebServerClass::RoutesRelay() {
    // リレー状態を更新（POSTメソッド）
    this->serverAsync.on("/post-relay", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL,
        std::bind(
            &WebServerClass::postRelay, this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4,
            std::placeholders::_5
        )
    );

    // リレーデータをクエリ（JSON形式で返す）
    this->serverAsync.on("/query-relay", HTTP_GET,
        std::bind(
            &WebServerClass::queryDataRelay, this, std::placeholders::_1
        )
    );

    // リレー状態を取得
    this->serverAsync.on("/getRelayStatus", HTTP_GET,
        std::bind(
            &WebServerClass::readRelayState, this,
            std::placeholders::_1
        )
    );

    // リレー状態を更新（GETメソッド）
    this->serverAsync.on("/check", HTTP_GET,
        std::bind(
            &WebServerClass::checkRelayState, this, std::placeholders::_1
        )
    );

    // 自動灌水を更新
    this->serverAsync.on("/auto-watering", HTTP_GET,
        std::bind(
            &WebServerClass::AutoWatering, this, std::placeholders::_1
        )
    );

    // 手動灌水を更新
    this->serverAsync.on("/manual-watering", HTTP_GET,
        std::bind(
            &WebServerClass::ManualWatering, this, std::placeholders::_1
        )
    );
}

/**
 * @brief Webサーバーのすべてのルートを設定します。
 */
void WebServerClass::Routes() {
    RoutesSystem();
    RoutesRelay();
}