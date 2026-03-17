/**
 *  @file BootButton.cpp
 *  @version 1.0.1
 *  
 *  @author
 *  basyair7
 *  
 *  @date
 *  2026
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

#include "MicroBox/hardware/BootButton.h"
#include "MicroBox/externobj"

/**
 * @brief ブートボタンの状態を監視し、WiFiモードを切り替えます。
 * @details この関数は、ブートボタンの状態を定期的に監視し、ボタンが押されたときにWiFiモードを切り替えます。
 * 切り替え後のWiFiモードはEEPROMに保存され、システムは再起動されます。
 */
void BootButton::ChangeWiFiMode() {
    // ブートボタンの現在の状態を読み取る
    this->__current_btn_state = this->__bootBtn.digitalReadPushButton();
    if (this->__current_btn_state != this->__last_btn_state)
        this->__btn_change = true;

    // ボタン状態が変化した場合の処理
    if (this->__btn_change) {
        if (!this->__current_btn_state) {
            this->__wifi_state = !this->__wifi_state; // WiFiモードを切り替える
            this->__MyEEPROM.save_wifi_state(this->__wifi_state); // EEPROMに新しいWiFiモードの状態を保存
            delay(50);
            // 切り替え後のWiFiモードをシリアルモニタに表示する。
            Serial.print(F("WiFi Mode : "));
            Serial.println(
                this->__wifi_state ? F("MODE STA") : F("MODE AP")
            );

            otaDisplay = true;
            if (!xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100))) return;
            lcd.clear();
            lcd.print("WiFi Mode :", 0, 0);
            lcd.print(this->__wifi_state ? "MODE STA" : "MODE AP", 0, 1);    
            xSemaphoreGive(i2cMutex);

            delay(2000);
            ESP.restart(); // システムを再起動して新しいWiFiモードを適用する
        }
        // ボタン状態の変化フラグをリセットする
        this->__btn_change = false;
    }
    // 最後のボタン状態を更新する
    this->__last_btn_state = this->__current_btn_state;
}
