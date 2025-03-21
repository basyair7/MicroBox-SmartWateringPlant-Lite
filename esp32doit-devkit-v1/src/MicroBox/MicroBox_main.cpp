/**
 *  @file MicroBox_main.cpp
 *  @version 1.0.0
 *  @brief Main program impelementation for the Arduino-based project.
 *  @details Implement the setup and loop functions, initializing hardware,
 *           modules, and handling core program logic.
 *  @author
 *  basyair7
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "MicroBox_main.h"
#include "ThisRTOS.hpp"

// Include Sensor and Module Headers
#include "MicroBox/hardware/BootButton.h"
#include "MicroBox/hardware/LEDBoard.h"
#include "MicroBox/hardware/sensor/DHTProgram"
#include "MicroBox/hardware/sensor/SoilMoisture"
#include "MicroBox/hardware/RelayController"
#include "MicroBox/hardware/LCDdisplay"

// Include System Headers
#include "MicroBox/software/BlynkProgram.h"
#include "MicroBox/software/MyEEPROM"
#include "MicroBox/software/LFSMemory"
#include "MicroBox/software/SysHandlers"
#include "MicroBox/software/ProgramWiFi"
#include "MicroBox/software/WebServer"
#include "MicroBox/software/WateringSys"
#include "MicroBox/externobj"
#include "variable"

#include "envWiFi.h"

// Initialize modules and global variables
// Initializes Sensor Program
BootButton bootbtn = BootButton(BOOTBUTTON, INPUT); //!< Boot button utility
LEDBoard led_running, led_warning; //!< Led indikator program
LCDdisplay lcd = LCDdisplay(); //!< LCD utility module
DHTProgram dhtprog = DHTProgram(PIN_DHT, DHT22); //!< DHT sensor program
SoilMoisture soilmoisture; //!< Soil Moisture sensor management module
// RelayController relayController; //!< Relay management module

// Initializes System Program
MyEEPROM myeeprom_prog;  //!< EEPROM utility module
LFSMemory lfsprog;       //!< LittleFS management module
WateringSys wateringSys; //!< Watering System program

// Milliseconds trackers for task execution
unsigned long __lastMillis__ = 0, __lastTimeReboot__ = 0;
bool RebootState = false; //!< Tracks ESP reboot state

/**
 * @brief Task for running sensor update and watering control.
 * @param pvParameter Parameters for the task (not used).
 * @details This task continuously reads sensor data (Soil Moisture and DHT)
 *          and runs the watering system based on sensor readings and predefined logic.
 *          It also prints the sensor values to the serial monitor for debugging and monitoring purposes.
 */
void ThisRTOS::vTask1(void *pvParameter) {
    (void) pvParameter; // Unused parameter

    // Initialize sensor
    soilmoisture.begin(PIN_SMS);
    dhtprog.begin();

    while (true) {
        // Run soil moisture sensor and update readings
        soilmoisture.getData();

        // Run dht sensor and update readings
        dhtprog.running();

        // static unsigned long LastTimeRefreshMonitor = 0;
        static unsigned long LastTimeRefreshLCD = 0;
        // if ((unsigned long) (millis() - LastTimeRefreshMonitor) >= 1000L)
        // {
        //     LastTimeRefreshMonitor = millis();

        //     if (WiFi.getMode() == WIFI_AP || WiFi.status() == WL_CONNECTED) {
        //         Serial.println(F("**************************************\n"));
        //         Serial.print(F("WiFi mode : "));
        //         Serial.println(WiFi.getMode() == WIFI_STA ? "WIFI STA" : "WIFI AP");

        //         // Print DHT sensor readings
        //         Serial.println(F("**************************************\n"));
        //         Serial.println(F("DHT Sensor :"));
        //         Serial.print(F("Temperature : "));
        //         Serial.print(dhtprog.temperature);
        //         Serial.println(F("°C"));
        //         Serial.print(F("Humidity : "));
        //         Serial.print(dhtprog.humidity);
        //         Serial.println(F("%"));

        //         // Print SoilMoisture sensor readings
        //         Serial.println(F("**************************************\n"));
        //         Serial.println(F("Soil Moisture Sensor :"));
        //         Serial.print(F("Data : "));
        //         Serial.print(soilmoisture.value);
        //         Serial.println(F("%"));
        //     }
        // }

        if ((unsigned long) (millis() - LastTimeRefreshLCD) >= 1500L) {
            LastTimeRefreshLCD = millis();

            auto updateLCD = [](int state) {
                if (state <= 5) {
                    lcd.clear();
                    lcd.print("Temp: ", 0, 0);
                    lcd.print(String(dhtprog.temperature) + "*C");
                    lcd.print("Hum: ", 0, 1);
                    lcd.print(String(dhtprog.humidity) + "%");
                }

                if (state >= 5 && state <= 10) {
                    lcd.clear();
                    lcd.print("Auto Watering: ", 0, 0);
                    lcd.print(wateringSys.AutoWateringState ? "Enable" : "Disable", 0, 1);
                }
                
                if (state >= 10 && state <= 15) {
                    lcd.clear();
                    lcd.print("Watering State: ", 0, 0);
                    lcd.print(wateringSys.WateringProcess ? "Watering" : "Standby", 0, 1);
                }

                if (state >= 15 && state <= 20) {
                    lcd.clear();
                    lcd.print("WiFi mode: ", 0, 0);
                    lcd.print(WiFi.getMode() == WIFI_STA ? "STA" : "AP", 0, 1);
                }

                if (state >= 20 && state <= 25) {
                    lcd.clear();
                    String statusWiFiSta = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
                    lcd.print("Status: ", 0, 0);
                    lcd.print(WiFi.getMode() == WIFI_STA ? statusWiFiSta : "Unknown", 0, 1);
                }
            };

            static int lcdState = 0;
            updateLCD(lcdState);
            lcdState = (lcdState + 1) % 25;
        }

        // Delay the task for 100 miliseconds to control the task execution frequency
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Task for running Blynk updates and web server OTA updates.
 * @param pvParameter Parameters for the task (not used).
 * @details This taks continuously runs the Blynk update function, and
 *          if the WiFi mode is set to Access Point (AP), it also update
 *          the web server for over-the-air (OTA) updates. This ensures that
 *          the Blynk application remains responsive and the web server can
 *          handle OTA update when needed.
 */
void ThisRTOS::vTask2(void *pvParameter) {
    (void) pvParameter; // Unused parameter

    // Read WiFi state and initialize
    bool wifi_state = myeeprom_prog.read(ADDR_EEPROM_WIFI_MODE);
    // Read Auto Change WiFi
    bool _autoChangeState;
    lfsprog.readConfigState(AUTOCHANGE, &_autoChangeState);
    
    Serial.print(F("Auto Change WiFi MODE: "));
    Serial.println(_autoChangeState ? "Enable" : "Disable");

    // Initialize WiFi program
    ProgramWiFi.setup(
        WIFI_SSID_STA_DEFAULT, WIFI_PASS_STA_DEFAULT,
        WIFI_SSID_AP_DEFAULT, WIFI_PASS_AP_DEFAULT
    );
    ProgramWiFi.initWiFi(wifi_state);

    // Initialize Blynk Program
    BlynkSetup();

    // Initialize WebServer Program
    WebServer.ServerInit();

    while (true) {
        // Run the Blynk update function to keep the Blynk application responsive
        BlynkRun();

        // If the WiFi mode is set to Access Point (AP), update the web for OTA updates.
        if (WiFi.getMode() == WIFI_AP) {
            WebServer.UpdateOTAloop();
        }

        // Delay the task for 100 miliseconds to control the task execution frequency
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Task for updating system information and handling reboots.
 * @param pvParameter Parameters for the task (not used).
 */
void ThisRTOS::vTask3(void *pvParameter) {
    (void) pvParameter;

    while (true) {
        // Run the system reboot logic if necessary
        RebootSys::run(&__lastTimeReboot__, RebootState);

        // Execute automatic state change logic
        bootbtn.ChangeWiFiMode();
        AutoChangeState::run();

        // Execute process queue for RelayController
        RelayController::PROCESSQUEUE();

        wateringSys.run();

        // Delay the task for 100 miliseconds to control the task execution frequency
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Setup function for initializing harware and modules.
 * @param baud Baud Rate for serial communication.
 * @details This function is executed once at startup. It initializes:
 *          - Serial communication and EEPROM
 *          - WiFi, web server, and Blynk (if STA Mode)
 *          - RTOS Programs
 */
void MicroBox_Main::setup(unsigned long baud) {
    Serial.begin(baud); //!< Initialize serial communication

    // Initialize EEPROM
    myeeprom_prog.initialize();

    // Initialize LittleFS Program
    lfsprog.setupLFS();

    // Initialize hardware components
    lcd.init();
    RelayController::BEGIN();
    led_running.begin(LED_RUNNING);
    led_warning.begin(LED_WARNING);
    bootbtn.begin();
    
    
    // Create FreeRTOS task
    // ThisRTOS *rtos = new ThisRTOS;
    // Create Task and Running vTask 1
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask1(param);
    }, "Task 1", 4096, NULL, 1, NULL, APP_CPU_NUM);
    
    // Create Task and Running vTask 1
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask2(param);
    }, "Task 2", 4096, NULL, 1, NULL, APP_CPU_NUM);

    // Create Task and Running vTask 1
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask3(param);
    }, "Task 3", 4096, NULL, 1, NULL, PRO_CPU_NUM);

    // delete rtos;
}

void MicroBox_Main::loop() {
    /** 
     * The loop can contain other task or
     * functionalities that need to be performed
    */
}