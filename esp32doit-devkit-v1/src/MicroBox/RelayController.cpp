/**
 *  @file RelayController.cpp
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

#include "MicroBox/hardware/RelayController"
#include "MicroBox/externobj"

// initialization static variable
int RelayController::ID_RELAY;
uint8_t RelayController::PIN_IO_RELAY;
bool RelayController::RELAY_STATE, RelayController::OPTOCOUPLE;
String RelayController::LABEL_RELAY;

/**
 * @brief Initializes RelayController Program
 * @param _optocouple
 * @param _delay
 */
void RelayController::begin(bool _optocouple, uint32_t _delay) {
    this->optocouple = _optocouple;
    if (!_optocouple) {
        this->ON  = 0x1;
        this->OFF = 0x0;
    }

    Serial.println(F("\nInitialize Relay Pins :"));
    for (const auto &item : RELAY_PINS) {
        lfsprog.parseVarRelay(
            String(VAR_SWITCH) + String(item),
            &this->pins_io_relay,
            &this->relay_state,
            &this->id_relay,
            &this->label_relay
        );

        if (this->pins_io_relay == -1) {
            Serial.printf("Failed to intialize relay %s\n",
            this->label_relay.c_str());
            continue;
        }

        Serial.printf("%s (%d): (%d) (%s)\n\n",
            this->label_relay.c_str(), this->id_relay,
            this->pins_io_relay, RELAY_STATE_STR_BOOL(this->relay_state)
        );

        pinMode(this->pins_io_relay, OUTPUT);
        digitalWrite(this->pins_io_relay, this->relay_state ? this->ON : this-> OFF);
        delay(_delay);
    }
}

void RelayController::read(String relay_varName) {
    // parse data relay
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    // insert value to static
    RelayController::PIN_IO_RELAY = this->pins_io_relay;
    RelayController::RELAY_STATE  = this->relay_state;
    RelayController::ID_RELAY     = this->id_relay;
    RelayController::LABEL_RELAY  = this->label_relay;
}

void RelayController::read(uint8_t pin_relay) {
    this->read(String(VAR_SWITCH) + String(pin_relay));
}

void RelayController::write(const String &relay_varName, const bool &state, uint32_t _delay)
{
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    this->actionQueue.push({
        this->pins_io_relay,
        state,
        this->id_relay,
        this->label_relay,
        _delay,
        millis() + _delay,
        true
    });
}

void RelayController::write(const uint8_t &pin_relay, const bool &state, uint32_t _delay)
{
    this->write(String(VAR_SWITCH) + String(pin_relay), state, _delay);
}

void RelayController::write_without_save(const String &relay_varName, const bool &state, uint32_t _delay)
{
    lfsprog.parseVarRelay(
        relay_varName,
        &this->pins_io_relay,
        &this->relay_state,
        &this->id_relay,
        &this->label_relay
    );

    this->actionQueue.push({
        this->pins_io_relay,
        state,
        this->id_relay,
        this->label_relay,
        _delay,
        millis() + _delay,
        false
    });
}

void RelayController::write_without_save(const uint8_t &pin_relay, const bool &state, uint32_t _delay)
{
    this->write(String(VAR_SWITCH) + String(pin_relay), state, _delay);
}

void RelayController::executeAction(const RelayAction &action) {
    digitalWrite(action.pin_relay, action.state ? this->ON : this->OFF);
    if (action.saveState) {
        lfsprog.changeConfigState(String(VAR_SWITCH) + String(action.pin_relay), action.state);
    }
    Serial.println(F("\nExecute Relay Action"));
    this->showAction(
        action.id_relay, 
        action.pin_relay, 
        action.label, 
        action.state, 
        action.saveState ? "Yes" : "No"
    );
}

void RelayController::processQueue() {
    if (!this->actionQueue.empty()) {
        RelayAction action = this->actionQueue.front();
        if (millis() >= action.nextActionTime) {
            this->actionQueue.pop();
            this->executeAction(action);
        }
    }
}