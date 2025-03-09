/**
 * @file WateringSys.cc
 * @version 1.0.1
 * @author basyair7
 * @date 2024
 * 
 * @brief Implementation of the automated watering system.
 * 
 * This module provides functionality to control an automatic watering system 
 * based on specific conditions such as time, rain levels, and user configurations.
 * It ensures the watering process occurs only during predefined time intervals 
 * and prevents unnecessary watering during rain or within restricted intervals.
 * 
 * @copyright
 * Copyright (C) 2024, basyair7
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "MicroBox/WateringSys.h"
#include "MicroBox/externprog.h"

/**
 * @brief Calculates the next watering date based on the interval.
 * 
 * @return DateTime object representing the next watering date.
 */
DateTime WateringSys::getNextWateringDate() const {
    // Convert lastWateringDay (UNIX timestamp) to DateTime
    DateTime _lastWateringDate(this->lastWateringDay);

    // Add the interval in days to _lastWateringDate
    DateTime _nextWateringDate = _lastWateringDate + TimeSpan(INTERVALDAYSDEFAULT * 86400);

    // Return the next watering date
    return _nextWateringDate;
}

/**
 * @brief Initializes the watering system.
 * 
 * Retrieves the last recorded watering day from EEPROM and ensures 
 * the system starts with no active watering process.
 */
void WateringSys::begin() {
    this->__myEEPROM__.get(ADDR_EEPROM_SAVE_STATE_WATERING, this->lastWateringDay);
}

/**
 * @brief Main execution loop for the watering system.
 * 
 * This method evaluates the conditions for starting or stopping watering:
 * - Checks if automatic watering is enabled.
 * - Monitors rain levels and cancels watering if rain is sufficient.
 * - Starts watering at 07:00 and stops at 10:00 if conditions are met.
 * - Prevents watering within two days of the last watering.
 * - Updates the last watering day in EEPROM when necessary.
 */
void WateringSys::run() {
    if (millis() - this->__LastResetFlags__ >= 60000) {
        this->__LastResetFlags__ = millis();
        this->resetFlags();
    }
    
    // Ensure the function is executed only once every second
    if (millis() - this->__LastMillis1__ >= 1000) {
        this->__LastMillis1__ = millis();
        
        this->WateringProcess = this->__WateringProcess__();

        // Check if automatic watering is enabled
        lfsprog.readConfigState(AUTOWATERING, &this->AutoWateringState);
        if (!this->AutoWateringState) return;

        // Save last watering regardless of mode
        TimeSpan timeSinceLastWateringState;
        int currentHour, currentMinute;
        this->LastWatering(&timeSinceLastWateringState, &currentHour, &currentMinute);

        // Retrieve current rain level
        this->Rainlvl = rainCheck.value;
        
        // Skip watering if the interval is too short (e.g., less that 2 days)
        if (timeSinceLastWateringState.days() < INTERVALDAYSDEFAULT) return;

        // Cancel watering if rain levels are sufficient
        if (this->Rainlvl >= 50) {
            if (!this->hasRainCancelled) {
                Serial.println(F("Automatic watering was canceled due to rain."));
                this->hasRainCancelled = true;
            }
            this->stopWatering();
            return;
        }

        // Handle watering based on specific time intervals
        if (currentHour == 7 && currentMinute == 0) {
            if (this->_isWatering) return; // Watering already active
            this->startWatering();
        }
        else if (currentHour == 10 && currentMinute == 0 && this->_isWatering) {
            this->stopWatering();
            return; // Exit after stopping watering
        }
    }
}

/**
 * @brief Starts the watering process.
 * 
 * Activates all relays connected to the watering system and sets the system 
 * state to indicate active watering. Outputs a log message to confirm operation.
 */
void WateringSys::startWatering() {
    this->_isWatering = true;
    this->LastWatering();
    for (const auto &pins : RELAY_PINS) {
        RelayController::write_without_save(pins, true, 1000); // Turn on relays
    }
    if (!this->hasStarted) {
        Serial.println(F("Automatic Watering is started."));
        this->hasStarted = true;
    }
}

/**
 * @brief Stops the watering process.
 * 
 * Deactivates all relays connected to the watering system and resets the system 
 * state to indicate no active watering. Outputs a log message to confirm operation.
 */
void WateringSys::stopWatering() {
    this->_isWatering = false;
    for (const auto &pins : RELAY_PINS) {
        RelayController::write_without_save(pins, false, 1000); // Turn off relays
    }
    
    if (!this->hasCompleted) {
        Serial.println(F("Automatic watering is completed."));
        this->hasCompleted = true;
    }
}

void WateringSys::LastWatering(TimeSpan *time_last_watering, int *CurrentHour, int *CurrentMinute) {
    // Retrieve the current date and time
    DateTime now = rtcprog.now();

    // Update current hour and minute if pointers are provided
    if (CurrentHour != nullptr) {
        *CurrentHour = now.hour();
    }
    if (CurrentMinute != nullptr) {
        *CurrentMinute = now.minute();
    }

    // Check if time_last_watering pointer is provided
    if (time_last_watering != nullptr) {        
        // Calculate the time since last watering
        DateTime _lastWateringDate(this->lastWateringDay);
        *time_last_watering = now - _lastWateringDate;
    }
    else {
        // Update last watering day and save to EEPROM
        this->lastWateringDay = now.unixtime();
        __myEEPROM__.save_state_watering(this->lastWateringDay);
    }
}
