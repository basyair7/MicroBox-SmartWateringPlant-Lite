/**
 *  @file WebServerHandlers.cpp
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
#include "MicroBox/externobj"

String WebServerClass::file_buffer(String fileName) {
    this->file = openfile(fileName, LFS_READ);
    if (!this->file) {
        return "";
    }

    // read file content
    size_t fileSize = this->file.size();
    std::unique_ptr<char []> fileBuffer(new char[fileSize + 1]);
    this->file.readBytes(fileBuffer.get(), fileSize);
    fileBuffer[fileSize] = '\0';
    this->file.close();

    return String(fileBuffer.get());
}

void WebServerClass::RebootSys(AsyncWebServerRequest *req) {
    StaticJsonDocument<100> doc;
    String jsonRes = "";
    uint16_t codeRes = 200;

    doc["status"] = codeRes;
    doc["msg"] = "Server has been restart";

    serializeJson(doc, jsonRes);
    req->send_P(codeRes, APPJSON, jsonRes.c_str());

    __lastTimeReboot__ = millis();
    RebootState = true;
}

void WebServerClass::EnableBlynk(AsyncWebServerRequest *req) {
    StaticJsonDocument<100> doc;

    String jsonRes = "";
    uint16_t codeRes = 200;

    doc["status"] = codeRes;
    doc["msg"] = "Enable Blynk, Server has been restart";

    serializeJson(doc, jsonRes);
    req->send_P(codeRes, APPJSON, jsonRes.c_str());

    myeeprom_obj.save_wifi_state(true);
    delay(50);

    __lastTimeReboot__ = millis();
    RebootState = true;
}

void WebServerClass::UpdateAutoChangeWiFi(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;
    String message = "", jsonBuffer = "";
    int statusCode;

    if (req->hasArg("state")) {
        bool autoChangeStateWiFi = req->getParam("state")->value().toInt();
        lfsprog.changeConfigState(AUTOCHANGE, autoChangeStateWiFi);
        message = "OK";
        statusCode = 200;
    }
    else {
        message = "Missing parameter";
        statusCode = 400;
    }

    doc["status"] = statusCode;
    doc["message"] = message;
    serializeJson(doc, jsonBuffer);
    
    req->send_P(statusCode, APPJSON, jsonBuffer.c_str());
}

// page 404
void WebServerClass::handleNotFound(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;

    String jsonRes = "";
    uint16_t codeRes = 404;

    doc["status"] = codeRes;
    doc["msg"] = "File not found";
    doc["uri"] = req->url();
    doc["method"] = req->method() == HTTP_GET ? "GET" : "POST";
    doc["args_count"] = req->args();

    JsonObject args = doc.createNestedObject("args");
    for (uint8_t i = 0; i < req->args(); i++) {
        args[req->argName(i)] = req->arg(i);
    }

    serializeJson(doc, jsonRes);
    req->send_P(codeRes, APPJSON, jsonRes.c_str());
}

void WebServerClass::handleError_deserializeJson(
    const String &program, DeserializationError error,
    bool *errorState, int *statusCode, String *res
) {
    bool state = false;
    switch (error.code()) {
        case DeserializationError::Ok:
            if (statusCode != nullptr) *statusCode = 200;
            if (res != nullptr) *res = "OK";

            break;

        case DeserializationError::InvalidInput:
            state = true;
            if (statusCode != nullptr) *statusCode = 404;
            if (res != nullptr) {
                *res = "Deserialization Error: ";
                *res += error.c_str(),
                *res += " - Malformed JSON.";
            }

            break;
        
        case DeserializationError::NoMemory:
            state = true;
            if (statusCode != nullptr) *statusCode = 500;
            if (res != nullptr) {
                *res = "Deserialization Error: ";
                *res += error.c_str();
                *res += " - Insufficient Memory.";
            }

            break;
        
        case DeserializationError::EmptyInput:
            state = true;
            if (statusCode != nullptr) *statusCode = 400;
            if (res != nullptr) {
                *res = "Deserialization Error: ";
                *res += error.c_str();
                *res += " - No input provided.";
            }

            break;

        default:
            state = true;
            if (statusCode != nullptr) *statusCode = 400;
            if (res != nullptr) {
                *res = "Deserialization Error: ";
                *res += error.c_str();
            }
            
            break;
    }

    if (errorState != nullptr) *errorState = state;
    if (state) {
        Serial.println(F("Failed to deserializeJson"));
        Serial.print(F("Program: "));
        Serial.println(program);
        Serial.print(F("Message: "));
        Serial.println(error.c_str());
    }
}