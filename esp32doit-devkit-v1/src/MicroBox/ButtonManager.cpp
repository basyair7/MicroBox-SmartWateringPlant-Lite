/**
 *  @file ButtonManager.cpp
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

#include "MicroBox/software/ButtonManager"
#include "MicroBox/externobj"

/**
 * @brief Toggles the state of a relay when the corresponding button is pressed.
 * @param btn Reference to the button object.
 * @param state Reference to the relay state variable.
 * @param relayIndex Index of the relay in the RELAY_PINS array.
 */
void ButtonManagerClass::toggleRelay(PushButtonDigital &btn, bool &state, int relayIndex) {
    bool currentState = btn.digitalReadPushButton();
    if (currentState != this->lastRelay1) {
        if (!currentState) {
            state = !state;
            RelayController::WRITE(
                String(VAR_SWITCH) + String(RELAY_PINS[relayIndex]),
                state,
                1000
            );
        }
    }
}

/**
 * @brief Toggles the auto-watering system state when the corresponding button is pressed.
 */
void ButtonManagerClass::toggleAutoWatering() {
    bool currentState = btnAutoWatering.digitalReadPushButton();
    if (currentState != this->lastAutoWatering) {
        if (!currentState) {
            this->autoWateringState = !this->autoWateringState;
            lfsprog.changeConfigState(AUTOWATERING, this->autoWateringState);
        }
    }
    this->lastAutoWatering = currentState;
}

/**
 * @brief Toggles the LCD backlight state when the corresponding button is pressed.
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

/**
 * @brief Initializes buttons and retrieves stored states from EEPROM.
 */
void ButtonManagerClass::init() {
    btnRelay1.init();
    btnRelay2.init();
    btnAutoWatering.init();
    btnBacklight.init();

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
 * @brief Periodically updates button states and triggers appropriate actions.
 */
void ButtonManagerClass::update() {
    this->toggleRelay(btnRelay1, relayState1, 0);
    this->toggleRelay(btnRelay2, relayState2, 1);
    this->toggleAutoWatering();
    this->toggleBacklight();
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined (NO_GLOBAL_ButtonManager)
ButtonManagerClass ButtonManager;
#endif