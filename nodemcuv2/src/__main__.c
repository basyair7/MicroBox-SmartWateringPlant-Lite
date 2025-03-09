/** 
 *  @file __main__.c
 *  @version 1.0.1
 *  @brief Main program for the Arduino framework.
 *  @details This file contains the main entry of the Arduino program.
 *           Be careful when modifying this file, as it may affect the entire program's behavior.
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

#include "__main__.h"

void setup()
{
    // Initialize the main program with serial communication speed set to 9600 bps.
    // EEPROM program loading is disable (false).
    main_setup(9600, false);
}

void loop()
{
    main_loop();
}
