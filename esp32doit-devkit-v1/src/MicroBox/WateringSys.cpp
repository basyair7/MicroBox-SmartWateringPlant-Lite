/**
 *  @file WateringSys.cpp
 *  @version 1.0.0
 *  @date 2025
 *  @author basyair7
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

#include "MicroBox/software/WateringSys"
#include "MicroBox/externobj"
#include "variable"

/**
 * @brief Starts the watering process.
 * @details
 * Activate all relays connected to the watering system and sets the system
 * state to indicate active watering. Outputs a log message to confirm operation.
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
 * @brief Stops the watering process.
 * @details
 * Deactivates all relays connected to the watering system and resets the system
 * state to indicate no active watering. Outputs a log message to confirm operation.
 */
inline void WateringSys::stopWatering() {
    this->_isWatering = false;
    for (const auto &item : RELAY_PINS)
        RelayController::WRITE(item, false, 1000); // Turn off relays

    if (!this->hasCompleted && this->hasStarted) {
        Serial.println(F("Automatic watering is completed."));
        this->hasStarted = false;
        this->hasCompleted = true;
    }
}

void WateringSys::begin() {
    lfsprog.readConfigState(AUTOWATERING, &this->AutoWateringState);
}

/**
 * @brief Main execution loop for the watering system
 * @details
 * This method evaluates the conditions for starting or stopping watering:
 * - Checks if automatic watering is enabled.
 * - Monitors Soil Moisture levels
 */

void WateringSys::run() {
    // Ensure the function is executed only once every second
    if (millis() - _LastMillis1 >= 5000) {
        _LastMillis1 = millis();

        WateringProcess = this->wateringProcess();

        // check if automatic watering is enable
        lfsprog.readConfigState(AUTOWATERING, &this->AutoWateringState);
        if (!this->AutoWateringState) return;

        // Retrieve current SoilMoisture
        if (soilmoisture.value > WATERING_LVL_MAX) {
            this->stopWatering();
            return; // Exit after stopping watering
        }
        else if (soilmoisture.value < WATERING_LVL_MIN) {
            if (this->_isWatering) return; // Watering already active
            this->startWatering();
        }
    }
}

bool WateringSys::wateringProcess() const {
    int _countRelayOn = 0;
    // read relay state on or off
    for (const auto &pins : RELAY_PINS) {
        if (RelayController::RELAY_STATE_STR_INT(digitalRead(pins)))
            _countRelayOn++;
    }

    return (_countRelayOn <= 0) ? false : true;
}