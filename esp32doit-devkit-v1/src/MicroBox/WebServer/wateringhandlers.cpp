/**
 *  @file wateringhandlers.cpp
 *  @version 1.0.0
 *  @date 2025
 *  @author
 *  basyair7
 *  
 *  @copyright
 *  Copyright (C) 2025, basyair7
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
#include "MicroBox/hardware/RelayController"
#include "MicroBox/externobj"

void WebServerClass::AutoWatering(AsyncWebServerRequest *req) {
    StaticJsonDocument<50> jsonDoc;
    String resBuffer = "", message = "";
    int statusCode = 200;

    if (req->hasParam("state")) {
        bool state = req->getParam("state")->value().toInt();
        lfsprog.changeConfigState(AUTOWATERING, state);
        message = "OK";
    }
    else {
        message = "Missing parameter";
        statusCode = 400;
    }

    jsonDoc["status"] = statusCode;
    jsonDoc["message"] = message;
    serializeJson(jsonDoc, resBuffer);

    req->send_P(statusCode, APPJSON, resBuffer.c_str());
}

void WebServerClass::ManualWatering(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> jsonDoc;
    String resBuffer = "", message = "";
    int statusCode = 200;

    if (req->hasParam("state")) {
        bool state = req->getParam("state")->value().toInt();
        if (!wateringSys.AutoWateringState) {
            for (const auto &relay : RELAY_PINS) {
                this->updateRelayState(relay, state);
            }
            message = "OK";
        }
        else {
            message = "Auto Watering is Enable";
            statusCode = 400;
        }
    }
    else {
        message = "Missing parameters";
        statusCode = 400;
    }

    jsonDoc["status"] = statusCode;
    jsonDoc["message"] = message;
    serializeJson(jsonDoc, resBuffer);

    req->send_P(statusCode, APPJSON, resBuffer.c_str());
}