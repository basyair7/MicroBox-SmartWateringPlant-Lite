/** 
 *  @file RelayController.cc
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

#include "MicroBox/RelayController.h"
#include "MicroBox/externprog.h"

/**
 * @brief Initializes RelayController Program
 * @param _optocouple
 * @param __delay__
 */
void RelayController::__begin__(bool _optocouple, uint32_t __delay__) {
    if (!_optocouple) {
        this->ON  = 0x1;
        this->OFF = 0x0;
    }

    Serial.println(F("\nInitialize Relay Pins : "));
    for (const auto &i : RELAY_PINS) {
        lfsprog.parseVarRelay(
            PIN_IO_ALIAS[i], 
            &this->__PINS_IO_RELAY__,
            &this->__RELAY_STATE__,
            &this->__ID_RELAY__,
            &this->__LABEL_RELAY__
        );

        if (this->__PINS_IO_RELAY__ == -1) {
            Serial.printf("Failed to initialize relay %s\n", this->__LABEL_RELAY__.c_str());
            continue;
        }

        Serial.printf("%s (%d): (%d) (%s)\n\n", 
            this->__LABEL_RELAY__.c_str(), this->__ID_RELAY__,
            this->__PINS_IO_RELAY__, RELAY_STATE_STR_BOOL(this->__RELAY_STATE__)
        );
        pinMode(this->__PINS_IO_RELAY__, OUTPUT);
        digitalWrite(this->__PINS_IO_RELAY__, this->__RELAY_STATE__ ? this->ON : this->OFF);
        delay(__delay__);
    }
}

void RelayController::__read__(uint8_t relay) {
    this->__read__(PIN_IO_ALIAS[relay]);
}

void RelayController::__read__(String relay) {
    // parse data relay
    lfsprog.parseVarRelay(
        relay,
        &this->__PINS_IO_RELAY__,
        &this->__RELAY_STATE__,
        &this->__ID_RELAY__,
        &this->__LABEL_RELAY__
    );

    // insert value to static variable
    RelayController::PIN_IO_RELAY = this->__PINS_IO_RELAY__;
    RelayController::RELAY_STATE  = this->__RELAY_STATE__;
    RelayController::ID_RELAY     = this->__ID_RELAY__;
    RelayController::LABEL_RELAY  = this->__LABEL_RELAY__;
}

void RelayController::__write__(const uint8_t &relay, const bool &state, uint32_t __delay__)
{
    this->__write__(PIN_IO_ALIAS[relay], state, __delay__);
}

void RelayController::__write__(const String &relay, const bool &state, uint32_t __delay__)
{
    lfsprog.parseVarRelay(
        relay,
        &this->__PINS_IO_RELAY__,
        &this->__RELAY_STATE__,
        &this->__ID_RELAY__,
        &this->__LABEL_RELAY__
    );

    this->actionQueue.push({
        this->__PINS_IO_RELAY__,
        state, 
        this->__ID_RELAY__, 
        this->__LABEL_RELAY__,
        __delay__,
        millis() + __delay__,
        true
    });
}

void RelayController::__write_without_save__(const uint8_t &relay, const bool &state, uint32_t __delay__)
{
    this->__write_without_save__(PIN_IO_ALIAS[relay], state, __delay__);
}

void RelayController::__write_without_save__(const String &relay, const bool &state, uint32_t __delay__)
{
    lfsprog.parseVarRelay(
        relay,
        &this->__PINS_IO_RELAY__,
        &this->__RELAY_STATE__,
        &this->__ID_RELAY__,
        &this->__LABEL_RELAY__
    );

    this->actionQueue.push({
        this->__PINS_IO_RELAY__,
        state,
        this->__ID_RELAY__,
        this->__LABEL_RELAY__,
        __delay__,
        millis() + __delay__,
        false
    });
}

void RelayController::__processQueue__() {
    if (this->actionQueue.empty()) {
        return;
    }

    RelayAction action = this->actionQueue.front();
    if (millis() >= action.nextActionTime) {
        this->actionQueue.pop();
        this->__executeAction__(action);
    }
}

void RelayController::__executeAction__(const RelayAction &action) {
    digitalWrite(action.pin_relay, action.state ? this->ON : this->OFF);
    if (action.saveState) {
        lfsprog.changeStateRelay(PIN_IO_ALIAS[action.pin_relay], action.state);
    }
    Serial.println(F("\nExecute Relay Action"));
    this->showAction(action.id_relay, action.pin_relay, action.label, action.state, action.saveState ? "Yes" : "No");
}

int RelayController::ID_RELAY;
uint8_t RelayController::PIN_IO_RELAY;
bool RelayController::RELAY_STATE, RelayController::optocouple;
String RelayController::LABEL_RELAY;
