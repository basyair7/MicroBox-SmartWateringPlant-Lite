/**
 *  @file MicroBox_main.h
 *  @version 1.0.0
 *  @brief Main header file for the ESP32 Arduino Framework.
 *  @details This file serves as the main entry point for the ESP32 program.
 *           Modifying ths file requires caution, as changes may impact the behavior of the entire program.
 *  @author basyair7
 *  @date 2025
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Arduino.h>
#include "EraseEEPROM"

class MicroBox_Main {
    /****** ADD NEW FUNCTION HERE ******/
    // void __example__(void);
    
    /****** DON'T MODIFY THIS METHOD's FUNCTION (BE CAREFUL)! ******/
    protected:
        /**
         * @brief User-defined setup function (private).
         * @param baud Baud rate for serial communication.
         */
        void setup(unsigned long baud);

        /**
         * @brief User-defined loop function (private).
         */
        void loop();
};

/**
 *  @class Main
 *  @brief Main class that serves as the entry point for the program.
 *  @details This class handles the main setup and loop functionality for the Arduino framework.
 *           It also provides as mechanism to erase EEPROM data.
 */
class Main : protected MicroBox_Main {
    /****** DON'T MODIFY THIS METHOD's FUNCTION (BE CAREFUL)! ******/
    bool _erase_eeprom_prog = false;
    
    /**
     *  @brief Erase all data in the EEPROM.
     *  @param baud Baud rate for serial communication.
     */
    static void EraseEEPROM(unsigned long baud) {
        Serial.begin(baud);

        // Attempt to initialize the EEPROM
        if (!EraseEEPROM::BEGIN()) {
            Serial.println(F("Failed to initialize EEPROM!\nThe run() function cannot be executed."));
            return;
        }

        Serial.println(F("EEPROM successfully initialized."));

        // Erase all data in the EEPROM
        Serial.println(F("Erasing EEPROM..."));
        EraseEEPROM::RUN();
        Serial.println(F("EEPROM successfully erased."));
    }

    public:
        /**
         *  @brief Gets the singleton instance of the Main class.
         *  @return Reference to the singleton instance.
         */
        static Main &instance() {
            static Main instance;
            return instance;
        }

        /**
         *  @brief Initializes the program.
         *  @param baud Baud rate for serial communication.
         *  @param erase_eeprom Flag to erase EEPROM data (default: false)
         */
        static void SETUP(unsigned long baud, bool erase_eeprom = false)
        {
            if (erase_eeprom) {
                instance()._erase_eeprom_prog = erase_eeprom;
                Main::EraseEEPROM(baud);
            }

            if (!instance()._erase_eeprom_prog)
                instance().setup(baud);
        }

        /**
         * @brief Main loop function
         */
        static void LOOP() {
            if (!instance()._erase_eeprom_prog)
                instance().loop();
        }
};