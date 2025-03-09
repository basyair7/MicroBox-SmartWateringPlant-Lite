/**
 *  @file data_server.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include <ArduinoJson.h>
#include "MicroBox/WebServer.h"
#include "MicroBox/RelayController.h"
#include "MicroBox/externprog.h"
#include "MicroBox/RainCheck.h"

void WebServer::__getDataServer__(DynamicJsonDocument &doc) {
    StaticJsonDocument<500> dataRelay;

    JsonObject data = doc.createNestedObject("data_server");
    // uint8_t hour, minute;
    // rtcprog.time(&hour, &minute);

    data["date"]        = rtcprog.datestr();
    // data["time"]        = String(hour) + ":" + String(minute);
    data["time"]        = rtcprog.timestr();
    data["dht"]["temp"] = dhtprog.temperature;
    data["dht"]["hum"]  = dhtprog.humidity;
    data["rainlvl"]     = rainCheck.value;
    data["WateringState"] = wateringsys.WateringProcess;
    data["ScheduleWatering"] = wateringsys.NextWateringDate();
    
    this->__queryDataRelayStr__(dataRelay);
    doc["data_relay"] = dataRelay;
}

// webserver program
void WebServer::DataWebServer(AsyncWebServerRequest *req) {
    DynamicJsonDocument doc(500);
    String jsonBuffer = "";
    int codeRes = 200;
    
    doc["status"] = codeRes;
    doc["heap_memory"]["total_heap"] = String(__totalHeapMemory__, 2);
    doc["heap_memory"]["free_heap"] = String(__freeHeapMemory__, 2);
    this->__getDataServer__(doc);
    
    serializeJson(doc, jsonBuffer);
    doc.clear();
    req->send_P(codeRes, APPJSON, jsonBuffer.c_str());
}


// websocket program
void WebServer::handleDataServerWS(AsyncWebSocketClient *client) {
    DynamicJsonDocument response(500);
    response["event"] = "data_server";
    response["heap_memory"]["total_heap"] = String(__totalHeapMemory__, 2);
    response["heap_memory"]["free_heap"] = String(__freeHeapMemory__, 2);
    this->__getDataServer__(response);

    String jsonResponse = "";
    serializeJson(response, jsonResponse);
    response.clear();
    client->text(jsonResponse.c_str());
}
