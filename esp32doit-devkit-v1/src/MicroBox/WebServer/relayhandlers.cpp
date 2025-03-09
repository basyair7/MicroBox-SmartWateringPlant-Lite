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
    if (relayController.optocouple)
        return digitalRead(pinRelay) ? "" : "checked";
    else
        return digitalRead(pinRelay) ? "checked" : "";
}

void WebServerClass::updateRelayState(String pinRelay, bool state) {
    relayController.write(pinRelay, state, 1000);
}

void WebServerClass::queryDataRelayStr(StaticJsonDocument<500> &doc) {
    doc["auto"]["id"] = "auto";
    doc["auto"]["status"] = wateringSys.AutoWateringState;
    for (const auto &item : RELAY_PINS) {
        JsonObject relay = doc.createNestedObject(String(VAR_SWITCH) + String(item));
        relayController.read(item);

        relay["id"] = relayController.pins_io_relay;
        relay["status"] = relayController.relay_state_str_int(
            digitalRead(relayController.pins_io_relay));
    }
}

void WebServerClass::queryDataRelay(AsyncWebServerRequest *req) {
    String res = "";
    uint16_t statusCode = 200;
    StaticJsonDocument<500> jsonDoc;

    jsonDoc["status"] = statusCode;

    JsonObject relayObj = jsonDoc.createNestedObject("data_relay");
    for (const auto &item : RELAY_PINS) {
        JsonObject data_relay = relayObj.createNestedObject(String(VAR_SWITCH) + String(item));
        relayController.read(item);
        data_relay["pin"]    = relayController.pins_io_relay;
        data_relay["status"] = RELAY_STATE_STR_BOOL(relayController.relay_state);
        data_relay["id"]     = relayController.id_relay;
        data_relay["name"]   = relayController.label_relay;
    }

    serializeJson(jsonDoc, res);
    jsonDoc.clear();
    req->send_P(statusCode, APPJSON, res.c_str());
}

void WebServerClass::postRelay(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
{
    String body = String((char*)data).substring(0, len);
    Serial.printf("\nReceived data\t: %s\n", body.c_str());

    // process data JSON
    StaticJsonDocument<500> doc, jsonData;
    String res = "", handleErrorMsg = "";
    int statusCode = 0; bool errorState;
    
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

        serializeJson(doc, res);
    }
    else {
        // check auto watering state
        if (!wateringSys.AutoWateringState) {
            for (size_t i = 0; i < size_t(RELAY_PINS)/size_t(RELAY_PINS[0]); i++)
            {
                if (doc.containsKey(String(VAR_SWITCH) + String(RELAY_PINS[i]))) 
                {
                    bool relayState = doc[String(VAR_SWITCH) + String(RELAY_PINS[i])];
                    this->updateRelayState(String(VAR_SWITCH) + String(RELAY_PINS[i]), relayState);

                    jsonData[String(VAR_SWITCH) + String(RELAY_PINS[i])] = relayState;
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

    serializeJson(doc, res);
    req->send_P(statusCode, APPJSON, res.c_str());
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
                if (item == _pinOut.toInt()) {
                    this->updateRelayState(String(VAR_SWITCH) + String(item), relayState);
                    data[String(VAR_SWITCH) + String(item)] = relayState;
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