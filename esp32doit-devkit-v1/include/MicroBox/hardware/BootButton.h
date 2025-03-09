/**
 *  @file BootButton.h
 *  @version 1.0.0
 *  @author basyair7
 *  @date 2025
 * 
 *  @copyright
 *  Copyright (C) 2025, basyair7
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
    PushButtonDigital __bootBtn;
    MyEEPROM __MyEEPROM;
    bool __current_btn_state = false;
    bool __last_btn_state    = false;
    bool __btn_change        = false;
    bool __wifi_state        = false;

    public:
        BootButton(uint8_t pin, uint8_t mode) : __bootBtn(pin, mode)
        {
            /*TODO (Not yet implemented)*/
        }

        void begin() {
            this->__bootBtn.init();
            this->__wifi_state = this->__MyEEPROM.read(ADDR_EEPROM_WIFI_MODE);
        }

        void ChangeWiFiMode() {
            this->__current_btn_state = this->__bootBtn.digitalReadPushButton();
            if (this->__current_btn_state != this->__last_btn_state)
                this->__btn_change = true;

            if (this->__btn_change) {
                if (!this->__current_btn_state) {
                    this->__wifi_state = !this->__wifi_state;
                    this->__MyEEPROM.save_wifi_state(this->__wifi_state);
                    delay(50);
                    Serial.print(F("WiFi Mode : "));
                    Serial.println(
                        this->__wifi_state ? F("MODE STA") : F("MODE AP")
                    );
                    delay(2000);
                    ESP.restart();
                }
                this->__btn_change = false;
            }
            this->__last_btn_state = this->__current_btn_state;
        }
};