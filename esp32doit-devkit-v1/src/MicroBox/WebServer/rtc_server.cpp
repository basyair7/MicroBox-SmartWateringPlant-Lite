/**
 *  @file rtc_server.cpp
 *  @version 1.0.1
 *  @brief データサーバー関連のWebサーバー関数ファイル。
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

#include <ArduinoJson.h>
#include "MicroBox/software/WebServer"
#include "MicroBox/externobj"

void WebServerClass::__getRTCServer__(StaticJsonDocument<200> &doc) {
    JsonObject _datetime = doc.createNestedObject("datetime");
    if (!xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) return;
    _datetime["date"] = rtcprog.datestr();
    _datetime["time"] = rtcprog.timestr();
    xSemaphoreGive(i2cMutex);
}

void WebServerClass::RTCServer(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;
    String response = "";
    uint16_t statusCode = 200;

    doc["status"] = statusCode;
    this->__getRTCServer__(doc);

    serializeJson(doc, response);
    req->send_P(statusCode, APPJSON, response.c_str());
}

void WebServerClass::handleRTCServer(AsyncWebSocketClient *client) {
    StaticJsonDocument<200> response;
    response["event"] = "datetime";
    response["heap_memory"]["total_heap"] = String(ESP.getHeapSize(), 2);
    response["heap_memory"]["free_heap"] = String(ESP.getFreeHeap(), 2);
    this->__getRTCServer__(response);

    String jsonResponse = "";
    serializeJson(response, jsonResponse);
    response.clear();
    client->text(jsonResponse.c_str());
}