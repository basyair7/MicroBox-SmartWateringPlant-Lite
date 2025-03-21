/**
 *  @file relayhandlers.cpp
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

#include "MicroBox/software/LFSMemory"

/**
 *  __initialize_data_relay__
 *  @param doc DynamicJsonDocument& -> id, name, pin, status
 */
void LFSMemory::__initialize_data_relay__(DynamicJsonDocument &doc) {
    // initialize default value if file does not exists
    unsigned int id = 0;
    for (size_t i = 0; i < (sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0])); i++)
    {
        JsonObject _data = doc.createNestedObject(
            String(VAR_SWITCH) + String(RELAY_PINS[i]));
        _data["id"] = id;
        _data["name"] = VALUE_DEFAULT[i];
        _data["pin"] = RELAY_PINS[i];
        _data["status"] = false; // default status to false

        id++;
    }
}

/**
 * changeVarRelay
 * @param varName
 * @param value
 */
void LFSMemory::changeVarRelay(String varName, String value) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            data[varName]["name"] = value;
        }
    );
}

/**
 * changeStateRelay
 * @param varName
 * @param status
 */
void LFSMemory::changeStateRelay(String varName, bool status) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            data[varName]["status"] = status;
        }
    );
}

/**
 * parseVarRelay
 * @param varName
 * @param pin uint8_t pointer
 * @param state bool pointer
 * @param id int pointer
 * @param name String pointer
 */
void LFSMemory::parseVarRelay(const String &varName, uint8_t *pin, bool *status, int *id, String *name)
{
    DynamicJsonDocument doc(1024);
    File _file = openfile(this->file_config_relay, LFS_READ);
    if (!_file) {
        Serial.printf("Failed to open %s file for loading\n", this->file_config_relay.c_str());
        return;
    }

    // read file content
    size_t _fileSize = _file.size();
    if (_fileSize == 0) {
        _file.close();
        Serial.println(F("Config file empty, initializing with default values."));
        this->__initialize_data_relay__(doc);
        String __newConfig__ = "";
        serializeJson(doc, __newConfig__);
        this->writeconfig(this->file_config_relay, __newConfig__);
    }

    std::unique_ptr<char []> fileBuffer(new char[_fileSize + 1]);
    _file.readBytes(fileBuffer.get(), _fileSize);
    fileBuffer[_fileSize] = '\0';
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
    if (status != nullptr)
        *status = relay["status"];
    if (id != nullptr)
        *id = relay["id"];
    if (name != nullptr)
        *name = relay["name"].as<String>();
}