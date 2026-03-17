/**
 *  @file BootButton.h
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

#pragma once

#include <Arduino.h>
#include <pushbutton.h>
#include "../software/MyEEPROM"

class BootButton {
    PushButtonDigital __bootBtn; //< ブートボタンオブジェクト
    MyEEPROM __MyEEPROM; //< EEPROM管理オブジェクト

    // ボタン状態の管理変数
    bool __current_btn_state = false;
    bool __last_btn_state    = false;
    bool __btn_change        = false;
    bool __wifi_state        = false; //< WiFiモードの状態（STAモードかAPモードか）

    public:
        /**
         * @brief ブートボタンオブジェクトを初期化するコンストラクタです。
         * @param pin ブートボタンが接続されているGPIOピン番号。
         * @param mode ブートボタンの動作モード（例：INPUT_PULLUPなど）。
         */
        BootButton(uint8_t pin, uint8_t mode) : __bootBtn(pin, mode)
        {
            /*TODO (Not yet implemented)*/
        }

        /**
         * @brief ブートボタンを初期化し、WiFiモードの状態をEEPROMから読み取ります。
         * @details この関数は、ブートボタンの初期化を行い、EEPROMからWiFiモードの状態を読み取ります。
         * WiFiモードは、STAモード（クライアントモード）とAPモード（アクセスポイントモード）を切り替えるために使用されます。
         */
        void begin() {
            this->__bootBtn.init(); // ブートボタンの初期化
            this->__wifi_state = this->__MyEEPROM.read(ADDR_EEPROM_WIFI_MODE); // EEPROMからWiFiモードの状態を読み取る
        }

        /**
         * @brief ブートボタンの状態を監視し、WiFiモードを切り替えます。
         * @details この関数は、ブートボタンの状態を定期的に監視し、ボタンが押されたときにWiFiモードを切り替えます。
         * 切り替え後のWiFiモードはEEPROMに保存され、システムは再起動されます。
         */
        void ChangeWiFiMode();
};