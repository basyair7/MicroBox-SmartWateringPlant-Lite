/**
 *  @file RainCheck.h
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

class RainCheck {
    public:
        static int VALUE;
        int value;
        
    public:
        static RainCheck &instance() {
            static RainCheck instance;
            return instance;
        }

        static void __begin__(const uint8_t pinInput) {
            instance().begin(pinInput);
        }

        static void __getData__(bool mapping = true, uint32_t in_max = 1023)
        {
            instance().getData(mapping, in_max);
            RainCheck::VALUE = instance().value;
        }

    public:
        void begin(const uint8_t __pinIn__) {
            this->__pinIn__ = __pinIn__;
            this->__check_begin__ = true;
            pinMode(this->__pinIn__, INPUT);
        }

        void getData(bool mapping = true, uint32_t in_max = 1023) {
            if (!this->__check_begin__) {
                Serial.println(F("Error: RainCheck not initialized! Call RainCheck::begin() first"));
                this->value = -1;
                return;
            }

            int _raw_result = analogRead(this->__pinIn__);
            this->value = mapping ? map(_raw_result, 0, in_max, 0, 100) : _raw_result;
        }

    private:
        uint8_t __pinIn__;
        bool __check_begin__ = false;
};
