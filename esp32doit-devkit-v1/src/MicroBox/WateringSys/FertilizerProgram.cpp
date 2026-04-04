/**
 *  @file FertilizerProgram.cpp
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

#include "MicroBox/software/FertilizerProgram"
#include "MicroBox/externobj"

/**
 * @brief ポンプを制御する関数。
 * @param state state が true の場合、ポンプをオンにする。state が false の場合、ポンプをオフにする。
 */
inline void FertilizerProgram::run_pump(bool state) {
    digitalWrite(this->_pumpPin, state ? this->on : this->off);
}

/**
 * @brief 撹拌 モーターを制御する関数。肥料を均一に混ぜるために使用する。
 * @param state state が true の場合、モーターをオンにする。state が false の場合、モーターをオフにする。
 */
inline void FertilizerProgram::run_motor(bool state) {
    digitalWrite(this->_motorPin, state ? this->on : this->off);
}

/**
 * @brief TDS値を確認し、
 * 目標値に達しているかを判断する。
 * 目標値より低い場合、モーターを作動させる。
 * 目標値に達した場合、モーターを停止する。
 */
inline void FertilizerProgram::mix_fertilizer() {
    if (tdsprog.getTDSValue() < (PPM_TARGET - PPM_TOLERANCE)) {
        // TDS値がしきい値を超えた場合、モーターを駆動させる。
        this->run_motor(true);
        this->FertilizerChecked = false;
    } else {
        // それ以外の場合、モーターを停止する。
        this->run_motor(false);
        this->FertilizerChecked = true;
    }
}

/**
 * @brief ポンプを一定間隔で
 * オン・オフする関数。
 * ノンブロッキング方式で動作する。
 */
inline void FertilizerProgram::give_fertilizer(bool _run) {
    // 最終施肥日と設定された間隔に基づき、施肥の実行時刻かどうかを判定する。
    const uint32_t onTime  = 500;
    const uint32_t offTime = 1000;

    if (!_run) {
        run_pump(false);
        this->_pumpState = false;
        return;
    }

    // 現在時刻を取得し、ポンプ運転間隔を計算する。
    unsigned long currentTime = millis();
    unsigned long interval = _pumpState ? onTime : offTime;

    // 定義された間隔に基づいてポンプの状態を切り替える。
    if (this->FertilizerChecked && (currentTime - _pumpPreviousTime >= interval)) {
        _pumpPreviousTime = currentTime;
        _pumpState = !_pumpState;
        this->run_pump(_pumpState);
    }
}

/**
 * @brief 最後に肥料を与えてから経過した日数を計算する。
 */
uint32_t FertilizerProgram::getDaysPassed() const {
    DateTime now = rtcprog.now();
    return (now.unixtime() - this->lastFertilizerDay) / 86400UL;
}

/**
 * @brief 次回の施肥までの残り日数を取得する。
 */
uint32_t FertilizerProgram::getDaysRemaining() const {
    uint32_t passed = this->getDaysPassed();
    
    if (passed >= INTERVAL_FERTILIZER) 
        return 0;

    return INTERVAL_FERTILIZER - passed;
}

/**
 * @brief 次回の施肥予定日を取得する。
 */
uint32_t FertilizerProgram::getNextFertilizerDay() const {
    DateTime nextDay =  DateTime(this->lastFertilizerDay) + TimeSpan(INTERVAL_FERTILIZER, 0, 0, 0);
    return nextDay.unixtime();
}

/**
 * @brief 初期化処理を行う関数。
 * ピンの設定とEEPROMからのデータ読み込みを行う。
 */
void FertilizerProgram::begin(const uint8_t pumpPin, const uint8_t motorPin, bool _optocouple) {
    if (!_optocouple) {
        this->on = true;
        this->off = false;
    }

    this->_pumpPin = pumpPin;
    this->_motorPin = motorPin;

    pinMode(this->_pumpPin, OUTPUT);
    pinMode(this->_motorPin, OUTPUT);

    this->run_motor(false);
    this->run_pump(false);

    this->lastFertilizerDay = this->__myeeprom.read(ADDR_EEPROM_AUTO_FERTILIZER);
    if (this->lastFertilizerDay == 0)
        this->lastFertilizerDay = rtcprog.now().unixtime();
}

/**
 * @brief 肥料制御のメイン状態機械。
 * IDLE -> MIXING -> PUMPING -> FINISHED の順番で動作する。
 */
void FertilizerProgram::run(uint8_t hour, uint32_t _minute) {
    // Code to run the fertilizer program
    static unsigned long lastCheck = 0;

    if (millis() - lastCheck < 1000) return;
    lastCheck = millis();

    DateTime now = rtcprog.now();

    TimeSpan timeSinceLast = now - DateTime(this->lastFertilizerDay);

    switch (this->_state) {
        case IDLE:
        {
            // 待機状態。設定された時間と間隔に達した場合、MIXINGへ移行する。
            this->run_motor(false);
            this->run_pump(false);

            if (timeSinceLast.days() >= INTERVAL_FERTILIZER && now.hour() == hour && now.minute() == 0) {
                this->_mixStartUnix = now.unixtime();
                this->_state = MIXING;
            }
        }
        break;

        case MIXING:
        {
            // 目標TDSに達した場合、PUMPINGへ移行する。
            // しかし、300秒以上経過した場合は安全のためにモーターを停止する。
            this->mix_fertilizer();
            if (this->FertilizerChecked) {
                this->_pumpStartUnix = now.unixtime();
                this->_pumpPreviousTime = millis();
                this->_pumpState = false;

                this->_state = PUMPING;
            }
            else if (now.unixtime() - this->_mixStartUnix > 300) {
                this->run_motor(false);
                this->_state = FINISHED;
            }
        }
        break;

        case PUMPING:
        {
            // 一定時間だけ肥料を供給する状態。
            uint32_t elapsed = now.unixtime() - this->_pumpStartUnix;
            if (elapsed >= (_minute * 60UL)) {
                this->give_fertilizer(false);
                _state = FINISHED;
            }
            else {
                this->give_fertilizer(true);
            }
        }
        break;
        
        case FINISHED:
        {
            // 施肥完了。最終施肥時間を保存し、IDLEに戻る。
            this->lastFertilizerDay = rtcprog.now().unixtime();
            this->__myeeprom.save_state(ADDR_EEPROM_AUTO_FERTILIZER, lastFertilizerDay);
            this->_state = IDLE;
        }
        break;
    }
}
