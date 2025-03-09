/**
 *  @file relayhandlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/LFSProgram.h"
#include "MicroBox/RelayController.h"

/*! __initialize_data_relay__
 * @param doc DynamicJsonDocument& -> id, name, pin, status
*/
void LFSProgram::__initialize_data_relay__(DynamicJsonDocument &doc)
{
    // initialize default values if file does not exists
    unsigned int id = 0;
    for (int i = 0; i < (int)(sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0])); i++) {
        JsonObject __data__ = doc.createNestedObject(PIN_IO_ALIAS[RELAY_PINS[i]]);
        __data__["id"] = id;
        __data__["name"] = VALUE_DEFAULT[i];
        __data__["pin"] = RELAY_PINS[i];
        __data__["status"] = false; // default status to false
        
        id++;
    }
}

/*! changeVarRelay
 * @param varName
 * @param value
*/
void LFSProgram::changeVarRelay(String varName, String value) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            data[varName]["name"] = value;
        }
    );
}

/*! changePinRelay
 * @param varName
 * @param pin
*/
void LFSProgram::changePinRelay(String varName, uint8_t pin) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            data[varName]["pin"] = pin;
        }
    );
}

/*! changeStateRelay
 * @param varName
 * @param status
*/
void LFSProgram::changeStateRelay(String varName, bool status) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            data[varName]["status"] = status;
        }
    );
}

/*! parseVarRelay
 * @param varName
 * @param pin
 * @param state
 * @param id
 * @param name
*/
void LFSProgram::parseVarRelay(const String &varName, uint8_t *pin, bool *state, int *id, String *name)
{
    DynamicJsonDocument doc(500);
    File _file = openfile(this->file_config_relay, LFS_READ);
    if (!_file) {
        Serial.printf("Failed to open %s file for loading\n", this->file_config_relay.c_str());
        return;
    }

    // read file content
    size_t fileSize = _file.size();
    if (fileSize == 0) {
        _file.close();
        this->__initialize_data_relay__(doc);
        String __newConfig__ = "";
        serializeJson(doc, __newConfig__);
        this->writeconfig(this->file_config_relay, __newConfig__);
    }

    std::unique_ptr<char []> fileBuffer(new char[fileSize + 1]);
    _file.readBytes(fileBuffer.get(), fileSize);
    fileBuffer[fileSize] = '\0';
    _file.close();

    // get content
    DeserializationError error = deserializeJson(doc, fileBuffer.get());
    if (error) {
        this->handleError_deserializeJson(
            "parseVarRelay",
            error.c_str()
        );
        return;
    }

    JsonObject relay = doc[varName];
    if (pin != nullptr)
        *pin = relay["pin"];
    if (state != nullptr)
        *state = relay["status"];
    if (id != nullptr) 
        *id = relay["id"];
    if (name != nullptr)
        *name = relay["name"].as<String>();
}
