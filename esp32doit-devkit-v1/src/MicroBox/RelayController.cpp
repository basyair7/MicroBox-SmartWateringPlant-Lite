/**
 *  @file RelayController.cpp
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2026
 *  
 *  @copyright
 *  Copyright (C) 2026, basyair7
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "MicroBox/hardware/RelayController"
#include "MicroBox/externobj"

// 
int RelayController::ID_RELAY;
uint8_t RelayController::PIN_IO_RELAY;
bool RelayController::RELAY_STATE, RelayController::OPTOCOUPLE;
String RelayController::LABEL_RELAY;

/**
 * @brief RelayControllerの初期化を行う。
 * @details この関数は、リレーのピン設定を行い、リレーの状態を初期化します。リレーのピン設定は、LittleFSに保存された設定から読み取られます。
 *          また、リレーの状態を安定させるために、指定された遅延時間を使用して、リレーの状態を設定します。
 * @param _optocouple // オプトカプラーの使用有無を指定します。trueの場合、リレーはオプトカプラーを使用して制御されます。falseの場合、リレーは直接制御されます。
 * @param _delay // 遅延時間（ミリ秒単位）。リレーの状態を設定する前に待機する時間を指定します。
 */
void RelayController::begin(bool _optocouple, uint32_t _delay) {
    StaticJsonDocument<150> doc;
    String jsonBuf;
    this->optocouple = _optocouple;
    if (!_optocouple) {
        this->ON  = 0x1;
        this->OFF = 0x0;
    }

    Serial.println(F("\nInitialize Relay Pins :\n"));
    for (const auto &item : RELAY_PINS) {
        lfsprog.parseVarRelay(
            String(VAR_SWITCH) + String(item),
            &this->pins_io_relay,
            &this->relay_state,
            &this->id_relay,
            &this->label_relay
        );

        if (this->pins_io_relay == -1) {
            Serial.printf("Failed to intialize relay %s\n",
            this->label_relay.c_str());
            continue;
        }
        
        JsonObject relayData = doc.createNestedObject(String(VAR_SWITCH) + String(item));
        relayData["pin"] = this->pins_io_relay;
        relayData["status"] = RELAY_STATE_STR_BOOL(this->relay_state);
        relayData["label"] = this->label_relay;
        relayData["id"] = this->id_relay;

        serializeJson(doc, jsonBuf);
        Serial.printf("%s\n\n", jsonBuf.c_str());
        jsonBuf = "";
        doc.clear();

        pinMode(this->pins_io_relay, OUTPUT);
        digitalWrite(this->pins_io_relay, this->relay_state ? this->ON : this-> OFF);
        delay(_delay);
    }
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *          アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param relay_varName // リレー変数名。リレーの情報を取得するための変数名を指定します。
 * 
 */
void RelayController::read(String relay_varName) {
    // parse data relay
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    // insert value to static
    RelayController::PIN_IO_RELAY = this->pins_io_relay;
    RelayController::RELAY_STATE  = this->relay_state;
    RelayController::ID_RELAY     = this->id_relay;
    RelayController::LABEL_RELAY  = this->label_relay;
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *          アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param pin_relay // リレーピン番号。リレーの情報を取得するためのピン番号を指定します。
 * 
 */
void RelayController::read(uint8_t pin_relay) {
    this->read(String(VAR_SWITCH) + String(pin_relay));
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *          アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param relay_varName // リレー変数名。リレーの情報を取得するための変数名を指定します。
 * @param state // リレーの状態。リレーをONにする場合はtrue、OFFにする場合はfalseを指定します。
 * @param _delay // 遅延時間（ミリ秒単位）。リレーの状態を設定する前に待機する時間を指定します。
 */
void RelayController::write(const String &relay_varName, const bool &state, uint32_t _delay)
{
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    this->actionQueue.push({
        this->pins_io_relay,
        state,
        this->id_relay,
        this->label_relay,
        _delay,
        millis() + _delay,
        true
    });
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *          アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param pin_relay // リレーピン番号。リレーの情報を取得するためのピン番号を指定します。
 * @param state // リレーの状態。リレーをONにする場合はtrue、OFFにする場合はfalseを指定します。
 * @param _delay // 遅延時間（ミリ秒単位）。リレーの状態を設定する前に待機する時間を指定します。
 * 
 */
void RelayController::write(const uint8_t &pin_relay, const bool &state, uint32_t _delay)
{
    this->write(String(VAR_SWITCH) + String(pin_relay), state, _delay);
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *         アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param relay_varName // リレー変数名。リレーの情報を取得するための変数名を指定します。
 * @param state // リレーの状態。リレーをONにする場合はtrue、OFFにする場合はfalseを指定します。
 * @param _delay // 遅延時間（ミリ秒単位）。リレーの状態を設定する前に待機する時間を指定します。
 * 
 */
void RelayController::write_without_save(const String &relay_varName, const bool &state, uint32_t _delay)
{
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    this->actionQueue.push({
        this->pins_io_relay,
        state,
        this->id_relay,
        this->label_relay,
        _delay,
        millis() + _delay,
        false
    });
}

/**
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *         アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 * @param pin_relay // リレーピン番号。リレーの情報を取得するためのピン番号を指定します。
 * @param state // リレーの状態。リレーをONにする場合はtrue、OFFにする場合はfalseを指定します。
 * @param _delay // 遅延時間（ミリ秒単位）。リレーの状態を設定する前に待機する時間を指定します。
 * 
 */
void RelayController::write_without_save(const uint8_t &pin_relay, const bool &state, uint32_t _delay)
{
    this->write_without_save(String(VAR_SWITCH) + String(pin_relay), state, _delay);
}

/**
 * @brief RelayControllerのアクションを実行する関数。
 * @details この関数は、リレーのアクションを実行します。リレーの状態を更新し、必要に応じて状態を保存します。また、アクションの実行内容をシリアルモニターに表示します。
 * @param action RelayAction構造体。実行するリレーアクションの情報を含む構造体を指定します。
 */
void RelayController::executeAction(const RelayAction &action) {
    digitalWrite(action.pin_relay, action.state ? this->ON : this->OFF);
    if (action.saveState) {
        lfsprog.changeStateRelay(String(VAR_SWITCH) + String(action.pin_relay), action.state);
    }
    Serial.println(F("\nExecute Relay Action"));
    this->showAction(
        action.id_relay, 
        action.pin_relay, 
        action.label, 
        action.state, 
        action.saveState ? "Yes" : "No"
    );
}

/** 
 * @brief RelayControllerのプロセスキューを処理する関数。
 * @details この関数は、リレーのアクションキューを処理します。キューに格納されたアクションは、指定された遅延時間が経過した後に実行されます。
 *         アクションが実行されると、リレーの状態が更新され、必要に応じて状態が保存されます。
 */
void RelayController::processQueue() {
    while (!this->actionQueue.empty()) {
        RelayAction action = this->actionQueue.front();
        if (millis() >= action.nextActionTime) {
            this->actionQueue.pop();
            this->executeAction(action);
        } else {
            break;
        }
    }
}
