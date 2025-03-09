/**
 *  @file BlynkProgram.cc
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

// Include MicroBox Headers
#include "MicroBox/BlynkProgram.h"
#include "MicroBox/MyEEPROM.hh"
#include "MicroBox/externprog.h"

// Include Blynk Library
#include "envBlynk.h"
#include <BlynkSimpleEsp8266.h>

MyEEPROM __MyEEPROM__; //!< EEPROM utility module
BlynkTimer TIMER;      //!< Blynk timer object for periodic tasks

/**
 * @brief Reboots ESP from Blynk APP command.
 * @param V4 Virtual pin for reboot command.
 * @details When this virtual pin is set, the reboot state is triggered and 
 *          stored in the `RebootState` variable.
 */
BLYNK_WRITE(V4) {
    __lastTimeReboot__ = millis();
    RebootState = param.asInt() == 1 ? true : false;
}

/**
 * @brief Disables Blynk mode and restart ESP.
 * @param V3 Virtual pin for Blynk mode disable.
 * @details Saves the state to EEPROM, delays for a stable reboot, and triggeres a restart.
 */
BLYNK_WRITE(V3) {
    __MyEEPROM__.save_state_blynk((param.asInt() == 1) ? false : true);
    delay(50);
    __lastTimeReboot__ = millis();
    RebootState = true;
}

/**
 * @brief Manages manual watering via Blynk.
 * @param V2 Virtual pin for manual watering control.
 * @details Activates or deactivates relays controlling the watering system.
 *          Manual wateringis disabled if auto-watering is active.
 */
BLYNK_WRITE(V2) {
    if (wateringsys.AutoWateringState) {
        Blynk.virtualWrite(V1, wateringsys.AutoWateringState);
        Blynk.virtualWrite(V2, 0);
        return;
    }
    
    for (const auto &pin : RELAY_PINS) {
        RelayController::write_without_save(pin, param.asInt() == 1 ? false : true, 1000);
        wateringsys.LastWatering();
    }
}

/**
 * @brief Configures the auto-watering state via Blynk.
 * @param V1 Virtual pin for auto-watering configuration.
 * @details Updates the auto-watering configuration in the LittleFS file system.
 */
BLYNK_WRITE(V1) {
    lfsprog.changeConfigState(AUTOWATERING, param.asInt() == 1 ? true : false);
}

/**
 * @brief Sends rain sensor data to Blynk.
 * @details Periodically transmits the rain level to a virtual pin. 
 */
unsigned long __lastMillsSendDataBlynk__ = 0;
void sendRainLvl(void) {
    if ((unsigned long) (millis() - __lastMillsSendDataBlynk__) >= 100) {
        __lastMillsSendDataBlynk__ = millis();
        Blynk.virtualWrite(V0, rainCheck.value);
    }
}

void Blynk_setup() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.begin(
            BLYNK_AUTH_TOKEN, lfsprog.__SSID_STA__.c_str(), lfsprog.__PASSWORD_STA__.c_str()
        );
        TIMER.setInterval(100L, sendRainLvl);
    }
}

void Blynk_run() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.run();
        TIMER.run();
    }
}