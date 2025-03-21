/**
 *  @file relayhandlers.cpp
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

String WebServerClass::RelayChecked(uint8_t pinRelay) {
    if (RelayController::OPTOCOUPLE)
        return digitalRead(pinRelay) ? "" : "checked";
    else
        return digitalRead(pinRelay) ? "checked" : "";
}

void WebServerClass::updateRelayState(int pinRelay, bool state) {
    RelayController::WRITE(pinRelay, state, 1000);
}

void WebServerClass::queryDataRelayStr(StaticJsonDocument<500> &doc) {
    doc["auto"]["id"] = "auto";
    doc["auto"]["status"] = wateringSys.AutoWateringState;
    for (const auto &item : RELAY_PINS) {
        JsonObject relay = doc.createNestedObject(String(VAR_SWITCH) + String(item));
        RelayController::READ(item);

        relay["id"] = RelayController::ID_RELAY;
        relay["status"] = RelayController::RELAY_STATE_STR_INT(
            digitalRead(RelayController::PIN_IO_RELAY)
        );
    }
}

void WebServerClass::queryDataRelay(AsyncWebServerRequest *req) {
    String jsonres = "";
    uint16_t statusCode = 200;
    StaticJsonDocument<500> jsonDoc;

    jsonDoc["status"] = statusCode;

    JsonObject relayObj = jsonDoc.createNestedObject("data_relay");
    for (const auto &item : RELAY_PINS) {
        JsonObject data_relay = relayObj.createNestedObject(String(VAR_SWITCH) + String(item));
        RelayController::READ(item);
        data_relay["pin"]    = RelayController::PIN_IO_RELAY;
        data_relay["status"] = RELAY_STATE_STR_BOOL(RelayController::RELAY_STATE);
        data_relay["id"]     = RelayController::ID_RELAY;
        data_relay["name"]   = RelayController::LABEL_RELAY;
    }

    serializeJson(jsonDoc, jsonres);
    jsonDoc.clear();
    req->send_P(statusCode, APPJSON, jsonres.c_str());
}

void WebServerClass::postRelay(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
{
    String body = String((char*)data).substring(0, len);
    Serial.printf("\nReceived data\t: %s\n", body.c_str());

    // process data JSON
    DynamicJsonDocument doc(500), jsonData(500);
    String jsonres = "", handleErrorMsg = "";
    int statusCode = 200; bool errorState;
    
    // check data body
    this->handleError_deserializeJson(
        "postRelay",
        deserializeJson(doc, body),
        &errorState,
        &statusCode,
        &handleErrorMsg
    );

    if (errorState) {
        doc["status"] = statusCode;
        doc["msg"] = handleErrorMsg;

        serializeJson(doc, jsonres);
    }
    else {
        // check auto watering state
        if (!wateringSys.AutoWateringState) {
            for (size_t i = 0; i < sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0]); i++)
            {
                String varName = String(VAR_SWITCH) + String(RELAY_PINS[i]);
                if (doc.containsKey(varName)) 
                {
                    bool relayState = doc[varName];
                    this->updateRelayState(RELAY_PINS[i], relayState);

                    jsonData[varName] = relayState;
                }
            }
        }
        else {
            statusCode = 400;
            handleErrorMsg = "Auto Watering is Enable";
        }
    }

    // Build final response
    doc.clear();
    doc["status"] = statusCode;
    doc["msg"] = handleErrorMsg;

    // Only include "data" if no error occurred
    if (!errorState && !jsonData.isNull()) {
        doc["data_relay"] = jsonData;
    }

    serializeJson(doc, jsonres);
    req->send_P(statusCode, APPJSON, jsonres.c_str());
}

void WebServerClass::readRelayState(AsyncWebServerRequest *req) {
    StaticJsonDocument<500> doc, dataRelay;
    String resBuffer = "";
    int statusCode = 200;

    this->queryDataRelayStr(dataRelay);
    doc["status"] = statusCode;
    doc["data_relay"] = dataRelay;
    
    serializeJson(doc, resBuffer);
    doc.clear();
    req->send(statusCode, APPJSON, resBuffer);
}

void WebServerClass::checkRelayState(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;
    StaticJsonDocument<50> data;
    String res = "";
    int statusCode = 200;
    if (req->hasParam("pinout") && req->hasParam("state")) {
        String _pinOut = req->getParam("pinout")->value();
        String state   = req->getParam("state")->value();

        bool relayState = state.toInt();
        int _querycount = 0;

        // check auto watering state
        if (!wateringSys.AutoWateringState) {
            for (const auto &item : RELAY_PINS) {
                String varName = String(VAR_SWITCH) + String(item);
                if (item == _pinOut.toInt()) {
                    this->updateRelayState(item, relayState);
                    data[varName] = relayState;
                    res = "OK";
                    statusCode = 200;
                    _querycount++;
                }
            }

            if (_querycount <= 0) {
                res = "Invalid pin";
                statusCode = 400;
            }
        }
        else {
            res = "Auto Watering is Enable";
            statusCode = 400;
        }
    }
    else {
        res = "Missing parameters";
        statusCode = 400;
    }

    // Build final response
    String jsonBuffer = "";
    doc["status"] = statusCode;
    doc["msg"]    = res;

    if (!data.isNull()) {
        doc["data_relay"] = data;
    }
    
    serializeJson(doc, jsonBuffer);
    req->send_P(statusCode, APPJSON, jsonBuffer.c_str());
}