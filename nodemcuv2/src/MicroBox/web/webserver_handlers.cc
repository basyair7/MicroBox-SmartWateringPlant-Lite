/**
 *  @file webserver_handlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include <ArduinoJson.h>
#include "MicroBox/WebServer.h"
#include "MicroBox/externprog.h"

String WebServer::__file_buffer__(String fileName) {
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

void WebServer::RebootSys(AsyncWebServerRequest *req) {
    StaticJsonDocument<100> doc;
    String jsonResponse = "";
    uint16_t codeRes = 200;

    doc["status"] = codeRes;
    doc["msg"] = "Server has been restart";

    serializeJson(doc, jsonResponse);
    req->send_P(codeRes, APPJSON, jsonResponse.c_str());

    __lastTimeReboot__ = millis();
    RebootState = true;
}

void WebServer::EnableBlynk(AsyncWebServerRequest *req) {
    StaticJsonDocument<100> doc;

    String jsonResponse = "";
    uint16_t codeRes = 200;

    doc["status"] = codeRes;
    doc["msg"] = "Enable Blynk, Server has been restart";

    serializeJson(doc, jsonResponse);
    req->send_P(codeRes, APPJSON, jsonResponse.c_str());

    __MyEEPROM__.save_state_blynk(true);
    delay(50);
    
    __lastTimeReboot__ = millis();
    RebootState = true;
}

void WebServer::updateAutoChangeWiFi(AsyncWebServerRequest *req) {
    StaticJsonDocument<200> doc;
    String message = "", jsonBuffer = "";
    uint16_t statusCode;
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
void WebServer::__handle_not_found__(AsyncWebServerRequest *req) {
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

void WebServer::handleError_deserializeJson(const String &program, DeserializationError error, bool *errorState, int *statusCode, String *res) 
{
    bool state = false;
    switch (error.code()) {
        case DeserializationError::Ok:
            if (statusCode != nullptr) *statusCode = 200;
            if (res != nullptr) *res = "OK";
            
            break;

        case DeserializationError::InvalidInput:
            state = true;
            if (statusCode != nullptr) *statusCode = 400;
            if (res != nullptr) {
                *res = "Deserialization Error : ";
                *res += error.c_str();
                *res += " - Malformed JSON.";
            }
            
            break;

        case DeserializationError::NoMemory:
            state = true;
            if (statusCode != nullptr) *statusCode = 500;
            if (res != nullptr) {
                *res = "Deserialization Error : ";
                *res += error.c_str();
                *res += " - Insufficient memory.";
            }

            break;

        case DeserializationError::EmptyInput:
            state = true;
            if (statusCode != nullptr) *statusCode = 400;
            if (res != nullptr) {
                *res = "Deserialization Error : ";
                *res += error.c_str();
                *res += " - No input provided.";
            }
            
            break;

        default:
            state = true;
            if (statusCode != nullptr) *statusCode = 400;
            if (res != nullptr) { 
                *res = "Deserialization Error : ";
                *res += error.c_str();
            }
            
            break;
    }

    if (errorState != nullptr) *errorState = state;
    if (state) {
        Serial.println(F("Failed to deserializeJson"));
        Serial.print(F("Program : "));
        Serial.println(program);
        Serial.print(F("message : "));
        Serial.println(error.c_str());
    }
}
