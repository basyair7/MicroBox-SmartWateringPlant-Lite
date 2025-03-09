/**
 *  @file ThisRTOS.hpp
 *  @version 1.0.0
 *  @date 2025
 *  @author basyair7
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/task.h>

class ThisRTOS {
    public:
        void vTask1(void *pvParameter);
        void vTask2(void *pvParameter);
        void vTask3(void *pvParameter);
};