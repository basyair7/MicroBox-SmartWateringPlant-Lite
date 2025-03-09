/*! 
 *  @file __main__.h
 *  @version 1.0.1
 *  @brief Main C-style header file for the Arduino framework.
 *  @details This file provides C-style declarations for the main setup and loop functions.
 *           Do not modify this file unless necessary, as it is critical to the program's structure.
 *  @author 
 *  basyair7
 *  @date 
 *  2024
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

#include <Arduino.h> // Include Arduino core library

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the main program.
 * @param baudRate Baud rate for serial communication.
 * @param erase_eeprom Boolean flag indicating whether EEPROM should be erased (true) or not (false).
 */
void main_setup(unsigned long baudRate, bool erase_eeprom);

/**
 * @brief Executes the main program loop.
 * @details This function should be called continuously in the program's main execution loop.
 */
void main_loop();

#ifdef __cplusplus
}
#endif
