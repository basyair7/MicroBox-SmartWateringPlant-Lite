/**
 *  @file main.cpp
 *  @version 1.0.0
 *  @brief Main program for the Arduino framework.
 *  @details This file contains the main entry of the ESP32 program.
 *           Be careful when modifying this file, as it may affect the entrie program's behavior.
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

#include "MicroBox_main.h"

/**
 * @brief Initializes the program by delegating to the Main class setup.
 * @details This function serves as the `setup` implementation from `MicroBox_main.h`
 *          It calls the `Main::SETUP` function for user-defined initialization logic.
 * @param baudRate Baud rate for serial communication.
 * @param erase_eeprom Boolean flag to erase EEPROM data (default: false).
 */
void setup() {
    // Initialize the main program with serial communication speed set to 115200 bps.
    // EEPROm program loading is disable (false)
    Main::SETUP(115200, false);
}

/**
 * @brief Executes the main program loop by delegating to the Main class loop.
 * @details This function serves as the `loop` implementation from `MicroBox_main.h`
 */
void loop() {
    Main::LOOP();
}