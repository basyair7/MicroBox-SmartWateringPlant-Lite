/*! 
 *  @file __main__.hh
 *  @version 1.0.1
 *  @brief Main header file for the Arduino framework.
 *  @details This file serves as the main entry point for the Arduino program.
 *           Modifying this file requires caution, as changes may impact the behavior of the entire program.
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Arduino.h>
#include "MicroBox/EraseEEPROM.hh"

/**
 *  @class Main
 *  @brief Main class that serves as the entry point for the program.
 *  @details This class handles the main setup and loop functionality for the Arduino framework.
 *           It also provides a mechanism to erase EEPROM data.
*/
class Main {
    /****** ADD NEW FUNCTION HERE ******/
    // void __example__(void);
        
    /****** DON'T MODIFY THIS METHOD's FUNCTION (BE CAREFUL)! ******/
    public:
        bool _erase_eeprom_prog = false;

        /**
         * @brief Gets the singleton instance of the Main class.
         * @return Reference to the singleton instance.
         */
        static Main &instance() {
            static Main instance;
            return instance;
        }

        /**
         * @brief Initializes the program.
         * @param baudSerial Baud rate for serial communication.
         * @param erase_eeprom Flag to erase EEPROM data (default: false).
         */
        static void setup(unsigned long baudSerial, bool erase_eeprom = false) {
            if (erase_eeprom) {
                instance()._erase_eeprom_prog = erase_eeprom;
                Main::EraseEEPROM(baudSerial);
            }

            if (!instance()._erase_eeprom_prog) {
                instance().__setup__(baudSerial);
            }
        }

        /**
         * @brief Main loop function.
         */
        static void loop() {
            if (!instance()._erase_eeprom_prog) {
                instance().__loop__();
            }
        }

    private:
        /**
         * @brief Erases all data in the EEPROM.
         * @param _baudSerial Baud rate for serial communication.
         */
        static void EraseEEPROM(unsigned long _baudSerial) {
            Serial.begin(_baudSerial);

            // Attempt to initialize the EEPROM
            if (!EraseEEPROM::BEGIN()) {
                Serial.println(F("Failed to initialize EEPROM! The run() function cannot be executed."));
                return;
            }

            Serial.println(F("EEPROM successfully initialized."));

            // Erase all data in the EEPROM
            Serial.println(F("Erasing EEPROM..."));
            EraseEEPROM::RUN();
            Serial.println(F("EEPROM successfully erased."));
        }

    private:
        /**
         * @brief User-defined setup function (private).
         * @param _baudSerial Baud rate for serial communication.
         */
        void __setup__(unsigned long _baudSerial);

        /**
         * @brief User-defined loop function (private).
         */
        void __loop__();
};
