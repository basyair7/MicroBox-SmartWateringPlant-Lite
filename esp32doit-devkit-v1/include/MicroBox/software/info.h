/**
 *  @file info.h
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

#pragma once

#include <Arduino.h>

#define NAMEPROJECT     "MicroBOX (SmartFeeder)"
#define CODENAME        "MicroBox"

#define VERSIONPROJECT 	"1.0.0"
#define HWVERSION 		"1.0.0"
#define SWVERSION 		"1.0.0"
#define BUILDDATE 		"2/27/2025"
#define REGION			"INDONESIA"

class Info {
    protected:
        const String __NAME_PROJECT__    = NAMEPROJECT;
        const String __CODE_NAME__       = CODENAME;
        const String __VERSION_PROJECT__ = VERSIONPROJECT;
        const String __HW_VERSION__      = HWVERSION;
        const String __SW_VERSION__      = SWVERSION;
        const String __BUILD_DATE__      = BUILDDATE;
        const String __REGION__          = REGION;
};
