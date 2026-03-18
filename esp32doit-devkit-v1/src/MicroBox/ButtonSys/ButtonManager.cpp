/**
 *  @file ButtonManager.cpp
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

#include "MicroBox/software/ButtonManager"
#include "MicroBox/externobj"

/**
 * @brief 対応ボタン押下時に、リレー状態の切り替えを行う。
 * @param btn ボタンを表すPushButtonDigitalオブジェクトの参照する参照型パラメータ。
 * @param state リレーの現在の状態を表すbool型変数への参照。
 * @param lastState 前回のボタン状態を表すbool型変数への参照。
 * @param relayIndex リレーのインデックス（0または1）を指定する整数型パラメータ。
 * 
 */
void ButtonManagerClass::toggleRelay(PushButtonDigital &btn, bool &state, bool &lastState, int relayIndex) {
    bool currentState = btn.digitalReadPushButton();
    if (currentState != lastState) {
        if (!currentState) {
            state = !state;
            RelayController::WRITE(
                String(VAR_SWITCH) + String(RELAY_PINS[relayIndex]),
                state,
                1000
            );
        }
    }
    lastState = currentState;
}

/**
 * @brief 対応ボタン押下時に、自動給水の状態の切り替えを行う。
 * @details 自動給水の状態は、LFSProgクラスのchangeConfigStateメソッドを使用して更新される。
 * 
 */
void ButtonManagerClass::toggleAutoWatering() {
    bool currentState = btnAutoWatering.digitalReadPushButton();
    if (currentState != this->lastAutoWatering) {
        if (!currentState) {
            this->autoWateringState = !this->autoWateringState;
            wateringSys.AutoWateringState = this->autoWateringState;
            lfsprog.changeConfigState(AUTOWATERING, this->autoWateringState);
        }
    }
    this->lastAutoWatering = currentState;
}

/**
 * @brief 対応ボタン押下時に、LCDバックライトの状態の切り替えを行う。
 * @details LCDバックライトの状態は、EEPROMに保存され、切り替え後の状態はLCDに反映される。
 * 
 */
void ButtonManagerClass::toggleBacklight() {
    bool currentState = btnBacklight.digitalReadPushButton();
    if (currentState != this->lastBacklight) {
        if (!currentState) {
            this->backlightState = !backlightState;
            this->eeprom_obj.save_state(ADDR_EEPROM_BACKLIGHT_LCD, this->backlightState);
            lcd.backlight(this->backlightState);
        }
    }
    lastBacklight = currentState;
}

uint8_t ButtonManagerClass::updateDisplay(const uint8_t interval) {
    static bool lastState;
    static uint8_t count = interval;

    bool btn = btnDisplay.digitalReadPushButton();
    if (btn != lastState) 
        if (!btn) {
            count++;
            count = (count > interval ? 0 : count);
        }
    lastState = btn;

    return count;
}

/**
 * @brief ButtonManagerClassの初期化を行う。各ボタンの初期化と、EEPROMからの状態の読み込みを行う。
 * @details 各リレーの状態もEEPROMから読み込まれ、初期化される。
 */
void ButtonManagerClass::init() {
    btnRelay.init();
    btnAutoWatering.init();
    btnBacklight.init();
    btnDisplay.init();

    this->backlightState = this->eeprom_obj.read(ADDR_EEPROM_BACKLIGHT_LCD);
    this->autoWateringState = wateringSys.AutoWateringState;

    std::vector<bool> relayState;
    for (const auto &pin : RELAY_PINS) {
        RelayController::READ(pin);
        relayState.push_back(RelayController::RELAY_STATE);
    }
    this->relayState1 = relayState[0];
    this->relayState2 = relayState[1];
}

/**
 * @brief ButtonManagerClassの状態を更新する。各ボタンの状態をチェックし、対応するアクションを実行する。
 * @details 自動給水が有効な場合は、リレーの状態の切り替えは行われない。自動給水の状態の切り替えと、LCDバックライトの状態の切り替えもこのメソッド内で処理される。
 * 
 */
void ButtonManagerClass::update() {
    if (!this->autoWateringState) {
        this->toggleRelay(btnRelay, relayState1, lastRelay1, 0);
        this->toggleRelay(btnRelay, relayState2, lastRelay2, 1);
    }
    
    this->toggleAutoWatering();
    this->toggleBacklight();
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined (NO_GLOBAL_ButtonManager)
/**
 * @brief ButtonManagerClassのグローバルインスタンスを定義する。これにより、他のファイルからButtonManagerオブジェクトを直接使用できるようになる。
 * @details NO_GLOBAL_INSTANCESまたはNO_GLOBAL_ButtonManagerが定義されている場合、このグローバルインスタンスは定義されない。
 */
ButtonManagerClass ButtonManager;
#endif