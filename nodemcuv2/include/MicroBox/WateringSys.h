/**
 * @file WateringSys.h
 * @version 1.0.1
 * @author basyair7
 * @date 2024
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
#include "RelayController.h"
#include "DS3231rtc.hh"
#include "MyEEPROM.hh"
#include "LEDBoard.h"
#include "variable.h"

class WateringSys {
    public:
        bool WateringProcess = false;
        bool AutoWateringState = false;
        
        String NextWateringDate() const {
            DateTime nextWatering = this->getNextWateringDate();
            return String(nextWatering.day()) + "/" + String(nextWatering.month()) + "/" + String(nextWatering.year());
        }

        void LastWatering(
            TimeSpan *time_last_watering = nullptr,
            int *CurrentHour = nullptr,
            int *CurrentMinute = nullptr
        );
        
        void begin();
        void run();
        
    private:
        DateTime getNextWateringDate() const;
        bool __WateringProcess__() const {
            int _countRelayOn = 0;
            // read relay state on or off
            for (const auto &pins : RELAY_PINS) {
                if (RelayController::RELAY_STATE_STR_INT(digitalRead(pins))) {
                    _countRelayOn++;
                }
            }

            return (_countRelayOn <= 0) ? false : true;
        }
        
        void startWatering();
        void stopWatering();

    private:
        bool hasStarted = false;
        bool hasCompleted = false;
        bool hasRainCancelled = false;

        void resetFlags() {
            this->hasStarted = false;
            this->hasCompleted = false;
            this->hasRainCancelled = false;
        }

    private:
        unsigned long __LastMillis1__ = 0, __LastResetFlags__ = 0;
        uint32_t lastWateringDay;
        bool _isWatering = false;
        int Rainlvl;
        MyEEPROM __myEEPROM__;
};

