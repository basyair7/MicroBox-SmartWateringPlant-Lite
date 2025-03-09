/**
 *  @file data_server.cpp
 *  @version 1.0.0
 *  @date 2025
 *  @author basyair7
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

#include <ArduinoJson.h>
#include "MicroBox/software/WebServer"
#include "MicroBox/hardware/RelayController"
#include "MicroBox/externobj"

void WebServerClass::GetDataServer(DynamicJsonDocument &doc) {
    StaticJsonDocument<500> dataRelay;

    JsonObject data = doc.createNestedObject("data_server");

    data["dht"]["temp"] = dhtprog.temperature;
    data["dht"]["hum"] = dhtprog.humidity;
    data["soil_moisture"] = soilmoisture.value;
    data["watering_state"] = wateringSys.WateringProcess;

    this->queryDataRelayStr(dataRelay);
    doc["data_relay"] = dataRelay;
    dataRelay.clear();
}

// WebServer Program
void WebServerClass::DataWebServer(AsyncWebServerRequest *req) {
    DynamicJsonDocument doc(500);
    String jsonBuffer = "";
    int codeRes = 200;

    doc["status"] = codeRes;
    this->GetDataServer(doc);

    serializeJson(doc, jsonBuffer);
    doc.clear();
    req->send_P(codeRes, APPJSON, jsonBuffer.c_str());
}

// WebSocket program
void WebServerClass::handleDataServeWS(AsyncWebSocketClient *client) {
    DynamicJsonDocument res(500);
    String jsonRes = "";

    res["event"] = "data_server";
    this->GetDataServer(res);
    serializeJson(res, jsonRes);
    client->text(jsonRes.c_str());
    res.clear();
}