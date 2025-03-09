/**
 *  @file __main_brigde__.cc
 *  @version 1.0.1
 *  @brief Implementation of the main bridge for the Arduino framework.
 *  @details This file provides the bridge implementation for the main setup and loop functions. 
 *           It connects the `C` style interface defined in `__main__.h` with the C++ implementation 
 *           in `__main__.hh`. Avoid modifying this file unless absolutely necessary, as it may 
 *           disrupt the core program's behavior.
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

#include "__main__.hh"  // Include C++ style main header
#include "__main__.h"   // Include C style main header
#include "MicroBox/externprog.h"

// Constants for periodic logging
unsigned long lastMillis_PrintFreeHeap = 0;  // Timestamp for the last memory log
const uint32_t delay_PrintFreeHeap = 10000UL;  // Delay (in ms) between memory log outputs

// Variables for memory usage monitoring
float __freeHeapMemory__ = 0.0;             // Stores free heap memory (in KB)
float __totalHeapMemory__ = ESP.getFreeHeap() / 1024.0;  // Stores total heap memory at startup (in KB)

// Variables for CPU usage monitoring
unsigned long lastCpuCheck = 0;             // Timestamp for the last CPU usage calculation
unsigned long loopStart = 0;                // Marks the start of the loop execution
float cpuUsage = 0;                         // Stores CPU usage percentage

/**
 * @brief Calculates the CPU usage based on loop execution time.
 * @details This function measures the time taken by the program's main loop 
 *          and calculates CPU usage as a percentage of one-second intervals.
 */
void calculateCpuUsage() {
    unsigned long now = millis();  // Current timestamp in milliseconds
    unsigned long interval = 1000UL; // 1 second
    unsigned long loopTime = now - loopStart;

    if (now - lastCpuCheck >= interval) {  // Check if 1 second has passed
        cpuUsage = ((float)loopTime / interval) * 100.0;  // Calculate CPU usage percentage
        lastCpuCheck = now;       // Update the timestamp
        loopStart = millis();     // Reset loop start time
    }
}

/**
 * @brief Initializes the program by delegating to the Main class setup.
 * @param baudRate Baud rate for serial communication.
 * @param erase_eeprom Boolean flag to erase EEPROM data (default: false).
 * @details This function serves as the `main_setup` implementation from `__main__.h`. 
 *          It calls the `Main::setup` function for user-defined initialization logic.
 *          After initialization, it logs the available heap memory.
 */
void main_setup(unsigned long baudRate, bool erase_eeprom /* = false */) {
    // Temporarily disable the watchdog to configure it
    ESP.wdtDisable();  // Disable the watchdog timer before making further configurations
    // Enable the watchdog timer with a timeout of 8 seconds (default timeout value)
    ESP.wdtEnable(WDTO_8S);  // Set the watchdog timeout to 8 seconds

    // Call the user-defined setup logic
    Main::setup(baudRate, erase_eeprom);

    // Log the initial heap memory state
    Serial.println();
    Serial.print(F("Total Heap\t: "));
    Serial.print(__totalHeapMemory__, 2);
    Serial.println(F(" KB"));
    Serial.print(F("Free Heap\t: "));
    Serial.print(ESP.getFreeHeap() / 1024.0, 2);  // Convert bytes to KB
    Serial.println(F(" KB"));
}

/**
 * @brief Executes the main program loop by delegating to the Main class loop.
 * @details This function serves as the `main_loop` implementation from `__main__.h`. 
 *          It periodically logs memory usage and CPU utilization for debugging purposes.
 */
void main_loop() {
    // Feed the watchdog to indicate the program is running normally
    ESP.wdtFeed();  // Reset the watchdog timer to prevent a reset if the program is running correctly

    // Execute user-defined loop logic
    Main::loop();

    // Update CPU usage
    calculateCpuUsage();

    // Periodically log system performance metrics
    if (millis() - lastMillis_PrintFreeHeap >= delay_PrintFreeHeap) {
        lastMillis_PrintFreeHeap = millis();  // Update the timestamp

        // Update memory metrics
        __freeHeapMemory__ = ESP.getFreeHeap() / 1024.0;  // Convert bytes to KB

        // Log CPU usage and memory statistics
        Serial.println();
        Serial.println(F("**** CPU Usage ****"));
        Serial.print(F("CPU Speed\t: "));
        Serial.print(ESP.getCpuFreqMHz());
        Serial.println(F(" MHz"));
        Serial.print(F("CPU Usage\t: "));
        Serial.print(cpuUsage, 2);  // Print CPU usage percentage
        Serial.println(F(" %"));
        Serial.println();
        Serial.println(F("**** Memory Usage ****"));
        Serial.print(F("Total Heap\t: "));
        Serial.print(__totalHeapMemory__);
        Serial.println(F(" KB"));
        Serial.print(F("Free Heap\t: "));
        Serial.print(__freeHeapMemory__, 2);  // Print free heap memory
        Serial.println(F(" KB"));
        Serial.print(F("Usage Heap\t: "));
        Serial.print(__totalHeapMemory__ - __freeHeapMemory__, 2);  // Calculate used heap memory
        Serial.println(F(" KB"));
        Serial.println("\n");
    }
}
