/**
 *  @file BootButton.h
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
 * 
 *  @copyright
 *  Copyright (C) 2024, basyair7
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
#include "MyEEPROM.hh"

class BootButton {
    public:
        BootButton(uint8_t pin, uint8_t mode) : __bootButton__(pin, mode) { }

        void begin() {
            this->__bootButton__.init();
            this->__wifiState__ = __myEEPROM__.read(ADDR_EEPROM_BLYNK_MODE);
        }

        void ChangeWiFiMode() {
            this->__currentButtonState__ = this->__bootButton__.digitalReadPushButton();
            if (this->__currentButtonState__ != this->__lastButtonState__)
                this->__buttonChange__ = true;

            if (this->__buttonChange__) {
                if (this->__currentButtonState__ == LOW) {
                    this->__wifiState__ = !this->__wifiState__;
                    this->__myEEPROM__.save_state_blynk(this->__wifiState__);
                    delay(50);
                    Serial.print(F("WiFi Mode : "));
                    Serial.println(this->__wifiState__ ? F("MODE STA") : F("MODE AP"));
                    delay(2000);
                    ESP.reset();
                }
                this->__buttonChange__ = false;
            }
            this->__lastButtonState__ = __currentButtonState__;
        }

    private:
        PushButtonDigital __bootButton__;
        MyEEPROM __myEEPROM__;
        bool __currentButtonState__ = false;
        bool __lastButtonState__    = false;
        bool __buttonChange__       = false;
        bool __wifiState__          = false;
};
