/**
 *  @file rtc_server.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include <ArduinoJson.h>
#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

void WebServer::__getRTCServer__(StaticJsonDocument<200> &doc) {
    JsonObject _datetime = doc.createNestedObject("datetime");
    _datetime["date"] = rtcprog.datestr();
    _datetime["time"] = rtcprog.timestr();
}

void WebServer::RTCServer(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;
    String response = "";
    uint16_t statusCode = 200;

    doc["status"] = statusCode;
    this->__getRTCServer__(doc);

    serializeJson(doc, response);
    req->send_P(statusCode, APPJSON, response.c_str());
}

void WebServer::handleRTCServer(AsyncWebSocketClient *client) {
    StaticJsonDocument<200> response;
    response["event"] = "datetime";
    response["heap_memory"]["total_heap"] = String(__totalHeapMemory__, 2);
    response["heap_memory"]["free_heap"] = String(__freeHeapMemory__, 2);
    this->__getRTCServer__(response);

    String jsonResponse = "";
    serializeJson(response, jsonResponse);
    response.clear();
    client->text(jsonResponse.c_str());
}
