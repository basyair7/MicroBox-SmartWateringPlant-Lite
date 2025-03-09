/**
 *  @file LFSProgram.h
 *  @version 1.0.1
 *  @brief Class for managing LittleFS-based configuration and state handling.
 *  This class provides interfaces for reading, writing, and managing configuration 
 *  files and runtime states in an embedded system using LittleFS.
 *  @date 2024
 *  @author basyair7
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

#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <vector>
#include "variable.h"
#include "envWiFi.h"

extern "C" {
    #define LFS          LittleFS
    #define lfsAvailable LittleFS.available
    #define lfsIsExists  LittleFS.exists
    #define openfile     LittleFS.open
    #define opendir      LittleFS.openDir
    #define removeitem   LittleFS.remove
    #define LFS_READ     "r"
    #define LFS_WRITE    "w"
}

extern "C" {
    #define AUTOWATERING "AUTO_WATERING"
    #define AUTOCHANGE   "AUTO_CHANGE"
}

/**
 * @class LFSProgram
 * @brief Manages configurations and runtime states using LittleFS.
 */
class LFSProgram {
    public:
        // Default WiFi configurations
        String __SSID_STA__     = WIFI_SSID_STA_DEFAULT;
        String __PASSWORD_STA__ = WIFI_PSK_STA_DEFAULT;
        String __SSID_AP__      = WIFI_SSID_AP_DEFAULT;
        String __PASSWORD_AP__  = WIFI_PSK_AP_DEFAULT;

        // Program state variables
        bool __WATERING_MODE_STATE__ = false;
        bool __AUTO_CHANGE_MODE__    = false;

    public:
        /**
         * @brief Initialize the LittleFS system and program configurations.
         */
        void setupLFS(void);

        /**
         * @brief Reinitialize program state variables from storage.
         */
        void reinitializeState(void);

        /**
         * @brief Reinitialize WiFi configurations from storage.
         */
        void reinitializeWiFiConfig(void);

        /**
         * @brief Reinitialize relay configurations from storage.
         */
        void reinitializeVarRelay(void);

        /**
         * @brief Modify a relay variable's value.
         * @param varName Name of the variable to change.
         * @param value New value for the variable.
         */
        void changeVarRelay(String varName, String value);

        /**
         * @brief Modify the pin associated with a relay.
         * @param varName Name of the relay variable.
         * @param pin New pin value.
         */
        void changePinRelay(String varName, uint8_t pin);

        /**
         * @brief Modify the state of a relay.
         * @param varName Name of the relay variable.
         * @param state New state value.
         */
        void changeStateRelay(String varName, bool state);

        /**
         * @brief Parse relay configuration details.
         * @param varName Name of the relay variable.
         * @param pin Pointer to store the pin value.
         * @param state Pointer to store the state value.
         * @param id Optional pointer to store the relay ID.
         * @param name Optional pointer to store the relay name.
         */
        void parseVarRelay(
            const String &varName, 
            uint8_t *pin, 
            bool *state, 
            int *id = nullptr, 
            String *name = nullptr
        );

        /**
         * @brief Update relay data using a JSON document.
         * @param doc JSON document containing relay data.
         */
        void __initialize_data_relay__(DynamicJsonDocument &doc);

        /**
         * @brief Change STA WiFi credentials.
         * @param new_ssid_sta New SSID for STA mode.
         * @param new_psk_sta New password for STA mode.
         */
        void changeConfigWiFi_STA(String new_ssid_sta, String new_psk_sta);

        /**
         * @brief Change AP WiFi credentials.
         * @param new_ssid_ap New SSID for AP mode.
         * @param new_psk_ap New password for AP mode.
         */
        void changeConfigWiFi_AP(String new_ssid_ap, String new_psk_ap);

        /**
         * @brief Update a program state configuration.
         * @param stateConfig Name of the state variable.
         * @param value New value for the state variable.
         */
        void changeConfigState(String stateConfig, bool value);

        /**
         * @brief Read a program state configuration.
         * @param stateConfig Name of the state variable.
         * @param value Pointer to store the state value.
         */
        void readConfigState(String stateConfig, bool *value);

    private:
        void initializeOrUpdateWiFiConfig(const String &cfile, std::function<void (StaticJsonDocument<500>&)> updateFunc);
        void initializeOrUpdateState(const String &cfile, std::function<void (StaticJsonDocument<200>&)> updateFunc);
        void initializeOrUpdateVarRelay(const String &cfile, std::function<void (DynamicJsonDocument&)> updateFunc);

        void initializeState(void);
        void initializeWiFiConfig(void);
        void initializeVarRelay(void);

        String readconfig(const String path);
        bool removefileconfig(const String path);
        void writeconfig(const String path, String valJson);
        void listFiles(void);

        /**
         * @brief List files in a directory.
         * @param path Directory path.
         */
        void PrintList(const String &path) {
            Serial.print(F("List Directory "));
            Serial.print(path);
            Serial.println(F(" :"));
            for (const auto &file : this->queryDirLFS(path)) {
                Serial.println(file);
            }
            Serial.println();
        }

        /**
         * @brief Retrieve file information from a directory.
         * @param path Directory path.
         * @return List of file names and sizes.
         */
        std::vector<String> queryDirLFS(const String path) {
            std::vector<String> files;
            Dir dir = LittleFS.openDir(path);
            while (dir.next()) {
                String fileInfo = dir.fileName();
                fileInfo += " - ";
                fileInfo += dir.fileSize();
                files.push_back(fileInfo);
            }
            return files;
        }

        /**
         * @brief Handle JSON deserialization errors.
         * @param program Name of the program encountering the error.
         * @param msg Error message.
         */
        void handleError_deserializeJson(const String &program, const String &msg) {
            Serial.println(F("Failed to deserializeJson"));
            Serial.print(F("Program : "));
            Serial.println(program);
            Serial.print(F("Message : "));
            Serial.println(msg);
        }

    private:
        const String file_config_wifi  = "/CONFIG/config_wifi.json";
        const String file_config_relay = "/CONFIG/config_relay.json";
        const String file_config_state = "/CONFIG/config_state.json";
};
