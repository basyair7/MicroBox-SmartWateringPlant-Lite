/**
 *  @file BlynkProgram.cpp
 *  @version 1.0.0
 *  
 *  @author
 *  basyair7
 *  
 *  @date
 *  2025
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

#include "MicroBox/software/BlynkProgram.h"
#include "MicroBox/software/ProgramWiFi"
#include "MicroBox/software/MyEEPROM"
#include "MicroBox/externobj"

// Include Blynk Library
#include "envBlynk.h"
#include <BlynkSimpleEsp32.h>

MyEEPROM myeeprom_obj; //!< EEPROM utility module
BlynkTimer Timer;      //!< Blynk timer object for periodic tasks

bool switch_state; //!< State variable for switch control

/**
 * @brief Reboot ESP from Blynk App Command.
 * @details When this virtual pin is set, the reboot state is triggered
 *          and stored in the `RebootState` variable.
 * @param V4 Virtual pin for reboot command.
 */
// BLYNK_WRITE(V4) {
//     __lastTimeReboot__ = millis();
//     RebootState = true;
// }

/**
 * @brief Disables Blynk mode or switch WiFi mode on WIFI_AP.
 * @details Saves the state to EEPROM, delays for a stable reboot,
 *          and triggeres a restart.
 * @param V4 Virtual pin for Blynk mode disable.
 */
BLYNK_WRITE(V4) {
    if (param.asInt() == 1) {
        myeeprom_obj.save_wifi_state(false);
        delay(50);
        __lastTimeReboot__ = millis();
        RebootState = true;
    }
}

/**
 * @brief Manages manual watering via Blynk.
 * @details Activates or deactivates relay controlling the watering system.
 *          Manual watering is disable if auto-watering is active.
 * @param V3 Virtual pin for manual watering control.
 */
BLYNK_WRITE(V3) {
    if (wateringSys.AutoWateringState) {
        Blynk.virtualWrite(V2, wateringSys.AutoWateringState);
        Blynk.virtualWrite(V3, wateringSys.WateringProcess);
        return;
    }

    for (const auto &pin : RELAY_PINS) {
        RelayController::WRITE(
            pin, 
            param.asInt() == 1 ? true : false,
            1000
        );
    }
}

/**
 * @brief Configures the auto-watering state via Blynk.
 * @details Updates the auto-watering configuration in the LittleFS file system.
 * @param V2 Virtual pin for auto-watering configuration.
 */
BLYNK_WRITE(V2) {
    lfsprog.changeConfigState(
        AUTOWATERING,
        param.asInt() == 1 ? true : false
    );
}

/**
 * @brief Sends data sensor Soil Moisture and DHT to Blynk.
 * @details Periodically transmits the soil moisture level
 *          and DHT temperature to a virtual pin.
 *          - V0 : Virtual pin for Soil Moisture sensor.
 *          - V1 : Virtual pin for DHT Sensor.
 */
unsigned long _LastMillisSendData = 0;
void sendDataSensor(void) {
    if ((unsigned long) (millis() - _LastMillisSendData) >= 100) {
        _LastMillisSendData = millis();
        Blynk.virtualWrite(V1, dhtprog.temperature);
        Blynk.virtualWrite(V0, soilmoisture.value);
    }
}

/**
 * @brief Initializes Blynk and sets up periodic tasks.
 * @details Connects to WiFi, initializes Blynk, and sets up a timer to
 *          send sensor data.
 */
void BlynkSetup() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.begin(
            BLYNK_AUTH_TOKEN,
            lfsprog.__SSID_STA__.c_str(),
            lfsprog.__PASS_STA__.c_str()
        );
        Timer.setInterval(500L, sendDataSensor);
    }
}

/**
 * @brief Runs the Blynk and timer tasks.
 * @details Continously runs the Blynk and timer tasks if the WiFi mode is STA.
 */
void BlynkRun() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.run();
        Timer.run();
    }
}
