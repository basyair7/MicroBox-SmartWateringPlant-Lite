/**
 *  @file relay_handlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/RelayController.h"
#include "MicroBox/externprog.h"
#include "MicroBox/variable.h"

String WebServer::RelayChecked(uint8_t pinRelay) {
    if (RelayController::optocouple) {
        return (digitalRead(pinRelay) ? "" : "checked");
    }
    else {
        return (digitalRead(pinRelay) ? "checked" : "");
    }
}

void WebServer::updateRelayState(String pinRelay, bool state) {
    // RelayController::write(pinRelay, state, 1000);
    RelayController::write_without_save(pinRelay, state, 1000);
    wateringsys.LastWatering();
}

void WebServer::__queryDataRelayStr__(StaticJsonDocument<500> &doc) {
    // read auto state
    // bool _check_autoWatering;
    // lfsprog.readConfigState(AUTOWATERING, &_check_autoWatering);

    doc["auto"]["id"] = "auto";
    doc["auto"]["status"] = wateringsys.AutoWateringState;
    for (const auto &i : RELAY_PINS) {
        JsonObject relay = doc.createNestedObject(PIN_IO_ALIAS[i]);
        RelayController::read(i);

        relay["id"] = PIN_IO_ALIAS[RelayController::PIN_IO_RELAY];
        relay["status"] = RelayController::RELAY_STATE_STR_INT(digitalRead(RelayController::PIN_IO_RELAY));
    }
}

void WebServer::queryDataRelay(AsyncWebServerRequest *req) {
    String res = "";
    int statusCode = 200;
    StaticJsonDocument<500> jsonDoc;

    jsonDoc["status"] = statusCode;

    JsonObject relayObj = jsonDoc.createNestedObject("data_relay");
    for (const auto &pin : RELAY_PINS) {
        JsonObject data_relay = relayObj.createNestedObject(PIN_IO_ALIAS[pin]);
        RelayController::read(pin);
        data_relay["pin"]    = RelayController::PIN_IO_RELAY;
        data_relay["status"] = RELAY_STATE_STR_BOOL(RelayController::RELAY_STATE);
        data_relay["id"]     = RelayController::ID_RELAY;
        data_relay["name"]   = RelayController::LABEL_RELAY;
    }

    serializeJson(jsonDoc, res);
    jsonDoc.clear();
    req->send_P(statusCode, APPJSON, res.c_str());
}

void WebServer::postRelay(AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
{
    String body = String((char*)data).substring(0, len);
    Serial.printf("\nReceived data\t: %s\n", body.c_str());

    // process data JSON
    String res = "", handleErrorMsg = "";
    int statusCode = 0; bool errorState;
    StaticJsonDocument<500> jsonDoc, dataDoc;

    // check data body
    this->handleError_deserializeJson(
        "postRelay",
        deserializeJson(jsonDoc, body),
        &errorState,
        &statusCode,
        &handleErrorMsg
    );

    if (errorState) {
        jsonDoc["status"] = statusCode;
        jsonDoc["msg"] = handleErrorMsg;

        serializeJson(jsonDoc, res);
    }
    else {        
        // check auto watering state
        if (!wateringsys.AutoWateringState) {
            // update relay status  
            for (int i = 0; i < (int)(sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0])); i++) {
                if (jsonDoc.containsKey(PIN_IO_ALIAS[RELAY_PINS[i]])) {
                    bool relayState = jsonDoc[PIN_IO_ALIAS[RELAY_PINS[i]]];
                    this->updateRelayState(PIN_IO_ALIAS[RELAY_PINS[i]], relayState);

                    dataDoc[PIN_IO_ALIAS[RELAY_PINS[i]]] = relayState;
                }
            }
        }
        else {
            statusCode = 400;
            handleErrorMsg = "Auto Watering is Enable";
        }
    }

    // Build final response
    jsonDoc.clear();
    jsonDoc["status"] = statusCode;
    jsonDoc["msg"]  = handleErrorMsg;

    // Only include "data" if no error occurred
    if (!errorState && !dataDoc.isNull()) {
        jsonDoc["data_relay"] = dataDoc;
    }

    serializeJson(jsonDoc, res);
    req->send_P(statusCode, APPJSON, res.c_str());
}

void WebServer::readRelayState(AsyncWebServerRequest *req) {
    StaticJsonDocument<500> doc, dataRelay;
    String resBuffer = "";
    int statusCode = 200;

    this->__queryDataRelayStr__(dataRelay);
    doc["status"] = statusCode;
    doc["data_relay"] = dataRelay;

    serializeJson(doc, resBuffer);
    doc.clear();
    req->send(statusCode, APPJSON, resBuffer);
}

void WebServer::checkRelayState(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> jsonDoc;
    StaticJsonDocument<50> dataDoc;
    String res = "";
    int statusCode = 200;
    if (req->hasParam("pinout") && req->hasParam("state")) {
        String _pinOut = req->getParam("pinout")->value();
        String state  = req->getParam("state")->value();

        bool relayState = state.toInt();
        int _querycount = 0;
        
        // check auto watering state
        if (!wateringsys.AutoWateringState) {
            for (const auto &i : RELAY_PINS) {
                if (PIN_IO_ALIAS[i] == _pinOut) {
                    this->updateRelayState(_pinOut, relayState);
                    dataDoc[PIN_IO_ALIAS[i]] = relayState;
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
    String jsonBuffer;
    jsonDoc["status"] = statusCode;
    jsonDoc["msg"]    = res;
    
    if (!dataDoc.isNull()) {
        jsonDoc["data_relay"] = dataDoc;
    }

    serializeJson(jsonDoc, jsonBuffer);
    req->send_P(statusCode, APPJSON, jsonBuffer.c_str());
}
