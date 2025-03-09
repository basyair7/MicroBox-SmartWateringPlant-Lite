/**
 *  @file watering_handlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"
#include "MicroBox/RelayController.h"
#include "MicroBox/externprog.h"

void WebServer::AutoWatering(AsyncWebServerRequest *req) {
    StaticJsonDocument<50> jsonDoc;
    String resBuffer = "", message = "";
    int statusCode = 200;

    if (req->hasParam("state")) {
        bool state = req->getParam("state")->value().toInt();
        lfsprog.changeConfigState(AUTOWATERING, state);
        message = "OK";
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

void WebServer::ManualWatering(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> jsonDoc;
    String resBuffer = "", message = "";
    int statusCode = 200;
    bool _check_watering_state;

    lfsprog.readConfigState(AUTOWATERING, &_check_watering_state);
    if (req->hasParam("state")) {
        bool state = req->getParam("state")->value().toInt();

        if (!_check_watering_state) {
            for (const auto &relay : RELAY_PINS) {
                this->updateRelayState(PIN_IO_ALIAS[relay], state);
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
