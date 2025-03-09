/**
 *  @file WebServer.h
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
 * 
 *  @copyright
 *  Copyright (C) 2024, basyair7
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once

#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <vector>
#include "LFSProgram.h"
#include "MyEEPROM.hh"
#include "info.h"

#define APPJSON     "application/json"
#define TEXTHTML    "text/html"
#define TEXTPLAIN   "text/plain"

class WebServer : protected Info {
    public:
        WebServer(int port = 80) : serverAsync(port), ws("/ws") { this->__PORT__ = port; }
        void ServerInit(void);
        void updateOTAloop(void);

    // index webserver
    private:
        void index(AsyncWebServerRequest *req);
        void DataWebServer(AsyncWebServerRequest *req);
        void RecoveryPage(AsyncWebServerRequest *req);
        void RTCServer(AsyncWebServerRequest *req);

        void __getDataServer__(DynamicJsonDocument &doc);
        void __getRTCServer__(StaticJsonDocument<200> &doc);
        void __run_css_js_webserver__(void);

    // handlers websocket
    private:
        void handleWebSocketMessage(void *args, uint8_t *data, size_t len);
        void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                    void *arg, uint8_t *data, size_t len);

        void handleDataServerWS(AsyncWebSocketClient *client);
        void handleToggleCheck(AsyncWebSocketClient *client, JsonDocument &doc);
        void handleRTCServer(AsyncWebSocketClient *client);
        
    // handlers webserver
    private:
        void EnableBlynk(AsyncWebServerRequest *req);
        void RebootSys(AsyncWebServerRequest *req);
        void ResetSys(AsyncWebServerRequest *req);
        void updateAutoChangeWiFi(AsyncWebServerRequest *req);
        
        void __handle_not_found__(AsyncWebServerRequest *req);
        String __file_buffer__(String fileName);

        void handleError_deserializeJson(
            const String &program,
            DeserializationError error,
            bool *errorState,
            int *statusCode = nullptr,
            String *res = nullptr
        );

    // handlers Watering state
    private:
        void AutoWatering(AsyncWebServerRequest *req);
        void ManualWatering(AsyncWebServerRequest *req);
        String StateChecked(bool x) {
            return (x ? "checked" : "");
        }

    // config relay
    private:
        String RelayChecked(uint8_t pinRelay);
        void updateRelayState(String pinRelay, bool state);

        void readRelayState(AsyncWebServerRequest *req);
        void __queryDataRelayStr__(StaticJsonDocument<500> &doc);
        void checkRelayState(AsyncWebServerRequest *req);

        void queryDataRelay(AsyncWebServerRequest *req);
        void postRelay(
            AsyncWebServerRequest *req,
            uint8_t *data,
            size_t len, size_t index, size_t total
        );

    // config wifi
    private:
        void WiFi_AP_Config_Main(AsyncWebServerRequest *req);
        void WiFi_STA_Config_Main(AsyncWebServerRequest *req);
        void Save_WiFi_AP_Config(AsyncWebServerRequest *req);
        void Save_WiFi_STA_Config(AsyncWebServerRequest *req);

        void __wifi_ap_config_1__(AsyncWebServerRequest *req);
        void __wifi_ap_config_2__(AsyncWebServerRequest *req);
        void __wifi_sta_config_1__(AsyncWebServerRequest *req);
        void __wifi_sta_config_2__(AsyncWebServerRequest *req);

    // config rtc
    private:
        void RTC_Config_Main(AsyncWebServerRequest *req);
        void Save_RTC_Config(AsyncWebServerRequest *req);

    // global config
    private:
        const String DIRCSS  = "/WEB/css/";
        const String DIRHTML = "/WEB/html/";
        const String DIRJS   = "/WEB/js/";

        File file;
        String __LOCALIP__;
        int __PORT__;
        AsyncWebServer serverAsync;
        AsyncWebSocket ws;
        MyEEPROM __MyEEPROM__;
};
