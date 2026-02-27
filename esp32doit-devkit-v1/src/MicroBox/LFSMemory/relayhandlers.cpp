/**
 *  @file relayhandlers.cpp
 *  @version 1.0.1
 *  @date 2026
 *  @author basyair7
 *  
 *  @brief  このファイルは、LFSMemoryクラスのリレーハンドラの実装を含む。
 *          リレーデータの初期化、リレー変数および状態の変更、ならびに設定ファイルからのリレー変数の解析機能を実装する。
 * 
 *  @copyright
 *  Copyright (C) 2026, basyair7
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
 *  @brief __initialize_data_relay__
 *  リレーのデータを初期化するための関数。設定ファイルが存在しない場合に、デフォルトのリレーデータを作成して設定ファイルに保存する。
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
 * @brief changeVarRelay
 * 変更するリレー変数の名前と新しい値を受け取り、設定ファイル内の対応するリレー変数の名前を更新します。
 * もしリレー変数が存在しない場合は、新しいリレー変数を作成して、指定された名前と値を設定します。
 * @param varName 変更するリレー変数の名前
 * @param value 新しいリレー変数の値
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
 * @brief changeStateRelay
 * リレーの状態を変更するための関数。リレー変数の名前と新しい状態を受け取り、設定ファイル内の対応するリレー変数の状態を更新します。
 * もしリレー変数が存在しない場合は、新しいリレー変数を作成して、指定された名前と状態を設定します。
 * @param varName 変更するリレー変数の名前
 * @param status 新しいリレー変数の状態 (true または false)
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
 * 設定ファイルからリレー変数の情報を解析するための関数。リレー変数の名前を受け取り、対応するリレー変数のピン番号、状態、ID、および名前を取得します。
 * もしリレー変数が存在しない場合は、引数のポインタにデフォルト値を設定します。
 * @param varName 解析するリレー変数の名前
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