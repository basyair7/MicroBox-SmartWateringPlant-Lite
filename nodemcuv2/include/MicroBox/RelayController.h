/**
 *  @file RelayController.h
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
#include <queue>
#include <vector>
#include "variable.h"

#define RELAY_STATE_STR_BOOL(state) ((state) ? "ON" : "OFF") // for boolean

struct RelayAction {
    uint8_t pin_relay;
    bool state;
    int id_relay;
    String label;
    uint32_t delay;
    uint32_t nextActionTime;
    bool saveState;
};

class RelayController {
    public:
        static int ID_RELAY;
        static uint8_t PIN_IO_RELAY;
        static bool RELAY_STATE;
        static String LABEL_RELAY;
        static bool optocouple;

        static const uint8_t RELAY_STATE_STR_INT(uint8_t pin) {
            if (RelayController::optocouple) {
                return pin ? 0 : 1;
            }
            else {
                return pin ? 1 : 0;
            }
        }

    public:
        static RelayController &instance() {
            static RelayController instance;
            return instance;
        }

        static void begin(bool optocouple = true, uint32_t delay = 1000UL) {
            RelayController::optocouple = optocouple;
            instance().__begin__(optocouple, delay);
        }
    
    public:
        static void read(uint8_t relay) {
            instance().__read__(relay);
        }
        static void read(String relay) {
            instance().__read__(relay);
        }
        static void write(const uint8_t &relay, const bool &state, uint32_t delay) {
            instance().__write__(relay, state, delay);
        }
        static void write(const String &relay, const bool &state, uint32_t delay) {
            instance().__write__(relay, state, delay);
        }
        static void write_without_save(const uint8_t &relay, const bool &state, uint32_t delay) {
            instance().__write_without_save__(relay, state, delay);
        }
        static void write_without_save(const String &relay, const bool &state, uint32_t delay) {
            instance().__write_without_save__(relay, state, delay);
        }
        static void processQueue() {
            instance().__processQueue__();
        }

    private:
        void __begin__(bool _optocouple, uint32_t __delay__);
    
    private:
        void __read__(uint8_t relay);
        void __read__(String relay);
        void __write__(const uint8_t &relay, const bool &state, uint32_t __delay__);
        void __write__(const String &relay, const bool &state, uint32_t __delay__);
        void __write_without_save__(const uint8_t &relay, const bool &state, uint32_t __delay__);
        void __write_without_save__(const String &relay, const bool &state, uint32_t __delay__);

        void __processQueue__();
        void __executeAction__(const RelayAction &action);

    private:
        void showAction(const int id, const uint8_t pin, const String &name, const bool state, const String &MsgSaveState) {
            Serial.println(F("Relay Action : "));
            Serial.print(F("Name\t\t: "));
            Serial.println(name);
            Serial.print(F("ID\t\t: "));
            Serial.println(id);
            Serial.print(F("PIN Relay\t: "));
            Serial.print(pin); Serial.print(F(" (")); Serial.print(PIN_IO_ALIAS[pin]); Serial.println(F(")"));
            Serial.printf("State\t\t: %s\n", state ? "ON" : "OFF");
            Serial.print(F("Save State\t: "));
            Serial.println(MsgSaveState);
        }

    private:
        std::queue<RelayAction> actionQueue;
        unsigned long LastActionTime = 0;
        uint8_t __PINS_IO_RELAY__;
        int __ID_RELAY__;
        bool __RELAY_STATE__;
        String __LABEL_RELAY__;

        uint8_t ON = 0x0;
        uint8_t OFF = 0x1;
};
