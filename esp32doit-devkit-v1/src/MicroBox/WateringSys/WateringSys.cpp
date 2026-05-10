/**
 *  @file WateringSys.cpp
 *  @version 1.0.1
 *  @date 2026
 *  @author basyair7
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

#include "MicroBox/software/WateringSys"
#include "MicroBox/externobj"
#include "variable"

/**
 * @brief 散水処理を開始する。
 * @details
 * この関数は、散水プロセスを開始します。リレーをONにして散水を開始し、システムの状態を更新します。
 * 散水が開始されたことをシリアルモニターに出力します。散水がすでに開始されている場合は、何も行いません。
 */
inline void WateringSys::startWatering() {
    this->_isWatering = true;
    for (const auto &item : RELAY_PINS)
        RelayController::WRITE(item, true, 1000);

    if (!this->hasStarted && this->hasCompleted) {
        Serial.println(F("Automatic watering is started."));
        this->hasStarted = true;
        this->hasCompleted = false;
    }
}

/**
 * @brief 散水処理を停止する。
 * @details
 * 散水システムに接続されたすべてのリレーをオフにし、システム状態を非アクティブな状態にリセットします。
 * 操作が完了したことをシリアルモニターに出力します。
 */
inline void WateringSys::stopWatering() {
    this->_isWatering = false;
    for (const auto &item : RELAY_PINS)
        RelayController::WRITE(item, false, 1000); // 全リレーをオフにする。

    if (!this->hasCompleted && this->hasStarted) {
        Serial.println(F("Automatic watering is completed."));
        this->hasStarted = false;
        this->hasCompleted = true;
    }
}

void WateringSys::handleSensorMode() {
    // 土壌水分センサーの値を監視し、設定された閾値に基づいて散水処理を開始または停止します。
    if (soilmoisture.value >= this->__WATERING_LVL_MAX__) {
        this->stopWatering();
        return; // 土壌水分値が上限しきい値以上の場合、散水を停止する。
    }

    if (soilmoisture.value <= this->__WATERING_LVL_MIN__) {
        if (this->_isWatering) return; // すでに散水中の場合は、何もせずに終了する。
        this->startWatering();
    }
}

void WateringSys::handleRTCMode() {
    int hour, minute;
    TimeSpan sinceLast;
    this->LastWatering(&sinceLast, &hour, &minute);

    if (sinceLast.days() < this->__INTERVAL_WATERING__) return;

    if (hour == 0 && minute == 0) this->hasSaveToday = false;

    bool inWindow = (hour >= this->__HOUR_START__ && hour < this->__HOUR_END__);

    if (inWindow) {
        if (!this->_isWatering && (soilmoisture.value <= this->__WATERING_LVL_MIN__)) 
        {
            this->startWatering();
        }
        else if (this->_isWatering && (soilmoisture.value >= this->__WATERING_LVL_MAX__))
        {
            this->stopWatering();
        }
    }

    if (hour == this->__HOUR_END__) {
        if (this->_isWatering) {
            this->stopWatering();
        }
        if (!this->hasSaveToday) {
            this->LastWatering();
            this->hasSaveToday = true;
        }
    }
}

/**
 * @brief WateringSysの初期化を行う。
 * @details 
 * この関数は、散水システムの初期化を行います。LittleFSから散水システムの設定を読み取り、散水システムの状態を初期化します。
 */
void WateringSys::begin() {
    lfsprog.readConfigState(AUTOWATERING, &this->AutoWateringState);
    eeprom_obj.get(ADDR_EEPROM_SAVE_STATE_WATERING, this->lastWateringDay);
}

/**
 * @brief 散水処理を実行する。
 * @details
 * この関数は、散水システムのメインループで呼び出されます。土壌水分センサーの値を監視し、設定された閾値に基づいて散水処理を開始または停止します。
 * 散水処理は、一定の時間間隔で実行されます。
 */

void WateringSys::run(int state) {
    if (millis() - this->_LastResetFlag >= 1000 * 60UL) {
        this->_LastResetFlag = millis();
        this->resetFlags();
    }

    // 散水プロセスの実行は、一定の時間間隔で行われます。
    if (millis() - _LastMillis1 >= 5000) {
        _LastMillis1 = millis();

        WateringProcess = this->wateringProcess();

        // 散水処理の実行は、土壌水分センサーの値に基づいて行われます。設定された閾値を超える場合は散水を停止し、閾値を下回る場合は散水を開始します。
        lfsprog.readConfigState(AUTOWATERING, &this->AutoWateringState);
        if (!this->AutoWateringState) return;

        switch(state) {
            case 1:
                this->handleSensorMode();
                break;
            case 2:
                this->handleRTCMode();
                break;
        }
    }
}

void WateringSys::LastWatering(TimeSpan *time_last_watering, int *CurrentHour, int *CurrentMinute) 
{
    DateTime now = rtcprog.now();

    if (CurrentHour != nullptr)   *CurrentHour = now.hour();
    if (CurrentMinute != nullptr) *CurrentMinute = now.minute();

    if (time_last_watering != nullptr) {
        DateTime lastDate(this->lastWateringDay);
        *time_last_watering = now - lastDate;
    } else {
        this->lastWateringDay = now.unixtime();
        eeprom_obj.save_state(ADDR_EEPROM_SAVE_STATE_WATERING, lastWateringDay);
    }
}

DateTime WateringSys::getNextWateringDate() const {
    DateTime _lastWateringDate(this->lastWateringDay);
    DateTime _nextWateringDate = _lastWateringDate + TimeSpan(this->__INTERVAL_WATERING__ * 86400);

    return _nextWateringDate;
}

bool WateringSys::wateringProcess() const {
    int _countRelayOn = 0;
    // 散水プロセスの状態を判断するために、すべてのリレーの状態を確認します。リレーがONになっている数をカウントし、その数に基づいて散水プロセスの状態を返します。
    for (const auto &pins : RELAY_PINS) {
        if (RelayController::RELAY_STATE_STR_INT(digitalRead(pins)))
            _countRelayOn++;
    }

    return (_countRelayOn <= 0) ? false : true;
}