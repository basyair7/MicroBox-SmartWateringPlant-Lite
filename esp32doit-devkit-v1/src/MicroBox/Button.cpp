/**
 *  @file Button.cpp
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

#include "MicroBox/software/Button.h"
#include "MicroBox/software/MyEEPROM"
#include "MicroBox/externobj"
#include "variable"

#include <vector>
#include <pushbutton.h>

MyEEPROM _eeprom_obj;

PushButtonDigital btn_relay_1 = PushButtonDigital(BTN_RELAY_1, INPUT);
PushButtonDigital btn_relay_2 = PushButtonDigital(BTN_RELAY_2, INPUT);
PushButtonDigital btn_auto_watering = PushButtonDigital(BTN_AUTOWATERING, INPUT);
PushButtonDigital btn_backlight = PushButtonDigital(BTN_BACKLIGHT, INPUT);

bool last_relay_state_1, last_relay_state_2;
bool change_relay_state_1, change_relay_state_2;
bool relay_state_1, relay_state_2;
bool last_auto_watering_state, change_auto_watering_state;
bool auto_watering_state;
bool last_backlight_state, change_backlight_state;
bool backlight_state;

void ChangeRelayState1(void) {
    bool currentState = btn_relay_1.digitalReadPushButton();

    if (currentState != last_relay_state_1)
        change_relay_state_1 = true;
    
    if (change_relay_state_1) {
        if (!currentState) {
            relay_state_1 = !relay_state_1;
            RelayController::WRITE(
                String(VAR_SWITCH) + String(RELAY_PINS[0]),
                relay_state_1,
                1000
            );
        }
        change_relay_state_1 = false;
    }
    last_relay_state_1 = currentState;
}

void ChangeRelayState2(void) {
    bool currentState = btn_relay_2.digitalReadPushButton();

    if (currentState != last_relay_state_2)
        change_relay_state_2 = true;
    
    if (change_relay_state_2) {
        if (!currentState) {
            relay_state_2 = !relay_state_2;
            RelayController::WRITE(
                String(VAR_SWITCH) + String(RELAY_PINS[1]),
                relay_state_2,
                1000
            );
        }
        change_relay_state_2 = false;
    }
    last_relay_state_2 = currentState;
}

void ChangeAutoWateringState(void) {
    bool currentState = btn_auto_watering.digitalReadPushButton();

    if (currentState != last_auto_watering_state)
        change_auto_watering_state = true;
    
    if (change_auto_watering_state) {
        if (!currentState) {
            auto_watering_state = !auto_watering_state;
            lfsprog.changeConfigState(AUTOWATERING, auto_watering_state);
        }
        change_auto_watering_state = false;
    }
    last_auto_watering_state = currentState;
}

void ChangeBacklightLCDState(void) {
    bool currentState = btn_backlight.digitalReadPushButton();

    if (currentState != last_backlight_state)
        change_backlight_state = true;

    if (change_backlight_state) {
        if (!currentState) {
            backlight_state = !backlight_state;
            _eeprom_obj.save_state(ADDR_EEPROM_BACKLIGHT_LCD, backlight_state);
            lcd.backlight(backlight_state);
        }
        change_backlight_state = false;
    }
    last_backlight_state = currentState;
}

void ButtonInit() {
    btn_relay_1.init();
    btn_relay_2.init();
    btn_auto_watering.init();
    btn_backlight.init();

    backlight_state = _eeprom_obj.read(ADDR_EEPROM_BACKLIGHT_LCD);

    std::vector<bool>listState;
    for (const auto &item : RELAY_PINS) {
        RelayController::READ(item);
        listState.push_back(RelayController::RELAY_STATE);
    }

    relay_state_1 = listState[0];
    relay_state_2 = listState[1];
    auto_watering_state = wateringSys.AutoWateringState;
}

void ButtonRunning() {
    ChangeRelayState1();
    ChangeRelayState2();
    ChangeAutoWateringState();
    ChangeBacklightLCDState();
}