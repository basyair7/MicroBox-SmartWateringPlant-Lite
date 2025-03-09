/**
 *  @file __main__.cpp
 *  @version 1.0.1
 *  @brief Main program implementation for the Arduino-based project.
 *  @details Implements the setup and loop functions, initializing hardware, 
 *           modules, and handling core program logic.
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

#include "__main__.hh"

// Include MicroBox Headers
#include "MicroBox/BootButton.h"
#include "MicroBox/LEDBoard.h"
#include "MicroBox/MyEEPROM.hh"
#include "MicroBox/SysHandlers.h"
#include "MicroBox/ProgramWiFi.h"
#include "MicroBox/LFSProgram.h"
#include "MicroBox/RainCheck.h"
#include "MicroBox/DS3231rtc.hh"
#include "MicroBox/RelayController.h"
#include "MicroBox/WateringSys.h"
#include "MicroBox/externprog.h"
#include "MicroBox/WebServer.h"
#include "MicroBox/BlynkProgram.h"

// Initialize modules and global variables
ProgramWiFi wifiprog;           //!< WiFi management module
LFSProgram lfsprog;             //!< LittleFS management module
MyEEPROM myeeprom_prog;         //!< EEPROM utility module
DS3231rtc rtcprog;              //!< RTC DS3231 utility module
RainCheck rainCheck;            //!< Rain sensor management module
WateringSys wateringsys;        //!< Watering System program
DHTProgram dhtprog(DHTSENSOR, DHT22); //!< DHT sensor program
BootButton bootbtn(BOOTBUTTON, INPUT); //!< Boot button utility
WebServer webServe(80);         //!< Web server on port 80

// Milliseconds trackers for task execution
unsigned long __lastMillis__ = 0, __lastTimeReboot__ = 0;
bool RebootState = false;       //!< Tracks ESP reboot state

/**
 * @brief Setup function for initializing hardware and modules.
 * @param _baudSerial Baud rate for serial communication.
 * @details This function is executed once at startup. It initializes:
 *          - Serial communication
 *          - EEPROM and RTC modules
 *          - Sensors (RainCheck and DHT)
 *          - LittleFS and RelayController
 *          - WiFi, web server and Blynk (if in STA mode)
 */
void Main::__setup__(unsigned long _baudSerial) {
    // Initialize serial communication
    Serial.begin(_baudSerial);

    // Initialize EEPROM and RTC modules
    myeeprom_prog.initialize();
    rtcprog.begin(true);
    
    Serial.println(F("RTC CHECKED : "));
    Serial.print(F("Date\t: "));
    Serial.println(rtcprog.datestr());
    Serial.print(F("Time\t: "));
    Serial.println(rtcprog.timestr());

    // Initialize hardware
    LEDBoard::begin();
    lfsprog.setupLFS();
    bootbtn.begin();

    // Read Blynk state and initialize WiFi
    bool x = myeeprom_prog.read(ADDR_EEPROM_BLYNK_MODE);

    // Initialize WiFi program with configurations from LittleFS
    wifiprog.setup(
        lfsprog.__SSID_STA__, lfsprog.__PASSWORD_STA__,
        lfsprog.__SSID_AP__, lfsprog.__PASSWORD_AP__
    );
    wifiprog.initWiFi(x);

    // Prevent sleep mode for stable operation
    wifiprog.optimizeWiFi(WIFI_NONE_SLEEP, true);
    
    // Initialize sensors and relay
    rainCheck.begin(RAINSENSOR);
    dhtprog.begin();
    RelayController::begin(true);

    // Initialize WateringSys Program
    wateringsys.begin();

    // Initialize Blynk Program
    Blynk_setup();
    
    // Call `ServerInit()` from the `webServe` object to initialize the web server.
    // This enables local configuration and OTA updates through the AP network.
    webServe.ServerInit();
}

/**
 * @brief Main loop function for the program.
 * @details This function is executed repeatedly to handle:
 *          - Changing WiFi modes
 *          - Processing relay control queues
 *          - Running periodic tasks (e.g., sensor readings)
 *          - Handle operations based on the current WiFi mode.
 */
void Main::__loop__() {
    // Handle boot button for changing WiFi modes
    bootbtn.ChangeWiFiMode();

    // Process relay control actions
    RelayController::processQueue();

    // run Blynk Program
    Blynk_run();
    
    // Execute the `updateOTAloop()` function of the `webServe` object to manage
    // Over-The-Air (OTA) firmware updates through a local web interface.
    webServe.updateOTAloop();

    // Execute periodic tasks every second
    if ((unsigned long)(millis() - __lastMillis__) >= 1000UL) {
        __lastMillis__ = millis();

        // run WateringSys program
        wateringsys.run();

        // Run sensor program
        dhtprog.running();
        rainCheck.getData();

        AutoChangeState::run();
    }
    
    RebootSys::run(&__lastTimeReboot__, RebootState);
    delay(100); // Stabilize loop timing
}
