/**
 *  @file LFSMemory.cpp
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

#include "MicroBox/software/LFSMemory"

/**
 * @brief List all file store in LittleFS, grouped by categories.
 * @details This function help in verifying stored files during setup.
 */

void LFSMemory::listFiles(void) {
    Serial.println(F("Listing file stored in LittleFS :"));
    this->PrintList("/");
    this->PrintList("/config");
    this->PrintList("/web");
    this->PrintList("/web/html");
    this->PrintList("/web/css");
    this->PrintList("/web/js");
}

/**
 * @brief Initializes or update WiFi configuration settings stored in a given file.
 * @details If the file is missing or corrupted, default WiFi credentials will be used.
 * 
 * @param cfile Configuration file name containing WiFi settings.
 * @param updateFunc Lambda function to update configuration data in the file.
 */
void LFSMemory::initializeOrUpdateWiFiConfig(const String &cfile, std::function<void (StaticJsonDocument<500>&)> updateFunc)
{
    StaticJsonDocument<500> data;
    String __readConfig__ = this->readconfig(cfile), __newConfig__ = "";
    if (__readConfig__ == "null" || !lfsIsExists(cfile)) {
        // Initialize with default WiFi credentials if the config is missing or corrupted.
        data[SSID_STA] = WIFI_SSID_STA_DEFAULT;
        data[PASS_STA] = WIFI_PASS_STA_DEFAULT;
        data[SSID_AP]  = WIFI_SSID_AP_DEFAULT;
        data[PASS_AP]  = WIFI_PASS_AP_DEFAULT;
    }
    else {
        DeserializationError error = deserializeJson(data, __newConfig__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateWiFiConfig", // Function name for error tracking
                error.c_str() // Error message
            );
            return;
        }
    }

    // Apply the changes using the provided lambda function
    updateFunc(data);

    // Serialize updated data and write it back to the file
    serializeJson(data, __newConfig__);
    this->writeconfig(cfile, __newConfig__);
}

/**
 * @brief Initializes or update relay configuration in the specified file.
 * @details If the is missing or corrupted, default relay settings will be used.
 * 
 * @param cfile Configuration file name containing relay settings.
 * @param updateFunc Lambda function to update configuration data in the file.
 */
void LFSMemory::initializeOrUpdateVarRelay(const String &cfile, std::function<void (DynamicJsonDocument&)> updateFunc)
{
    DynamicJsonDocument doc(500);
    String __readconf__ = this->readconfig(cfile), __newconf__ = "";
    
    if (__readconf__ == "null" || !lfsIsExists(cfile)) {
        // Initialize relay data with default values
        this->__initialize_data_relay__(doc);
    }
    else {
        DeserializationError error = deserializeJson(doc, __newconf__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateVarRelay", // Function name for error tracking
                error.c_str() // Error message
            );
            return;
        }
    }

    // Apply the change using the provided lambda function
    updateFunc(doc);

    // Serialize updated data and write it back to the file
    serializeJson(doc, __newconf__);
    this->writeconfig(cfile, __newconf__);
}

/**
 * @brief Initializes or updates system state settings in the specified configuration file.
 * @details Default state values are applied if the file is missing or corrupted.
 * 
 * @param cfile Configuration file name containing state settings.
 * @param updateFunc Lambda function to update configuration data in the file.
 */
void LFSMemory::initializeOrUpdateState(const String &cfile, std::function<void (StaticJsonDocument<200>&)> updateFunc)
{
    StaticJsonDocument<200> doc;
    String __readconfig__ = this->readconfig(cfile), __newconfig__ = "";
    
    if (__readconfig__ == "null" || !lfsIsExists(cfile)) {
        // Initialize with default state values if the config is missing or corrupted
        doc[AUTOWATERING] = false;
        doc[AUTOCHANGE]   = false;
    }
    else {
        DeserializationError error = deserializeJson(doc, __newconfig__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateState", // Function name for error tracking
                error.c_str() // Error message
            );
            return;
        }
    }

    // Apply the changes using the provided lambda function
    updateFunc(doc);

    // Serialize updated data and write it back to the file
    serializeJson(doc, __newconfig__);
    this->writeconfig(cfile, __newconfig__);
}

/**
 * @brief Initailizes WiFi configuration with values from the configuration file.
 */
void LFSMemory::initializeWiFiConfig(void) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            this->__SSID_STA__ = data[SSID_STA].as<String>();
            this->__SSID_AP__  = data[SSID_AP].as<String>();
            this->__PASS_STA__ = data[PASS_STA].as<String>();
            this->__PASS_AP__  = data[PASS_AP].as<String>();
        }
    );
}

/**
 * @brief Initailizes relay configuration using the stored data.
 */
void LFSMemory::initializeVarRelay(void) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            // Placeholder for relay configuration updates
        }
    );
}

/**
 * @brief Initializes system state settings from the configuration file.
 */
void LFSMemory::initializeState(void) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // Extract system state values from the configuration
            this->__WATERING_MODE_STATE__ = data[AUTOWATERING];
            this->__AUTO_CHANGE_MODE__    = data[AUTOCHANGE];
        }
    );
}

/**
 * @brief Reinitializes WiFi configuration with default values.
 */
void LFSMemory::reinitializeWiFiConfig(void) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            // Reset to default WiFi credentials
            this->__SSID_STA__  = WIFI_SSID_STA_DEFAULT;
            this->__SSID_AP__   = WIFI_SSID_AP_DEFAULT;
            this->__PASS_STA__  = WIFI_PASS_STA_DEFAULT;
            this->__PASS_AP__   = WIFI_PASS_AP_DEFAULT;

            // Update configuration data
            data[SSID_STA] = this->__SSID_STA__;
            data[SSID_AP]  = this->__SSID_AP__;
            data[PASS_STA] = this->__PASS_STA__;
            data[PASS_AP]  = this->__PASS_AP__;

            Serial.println(F("Reinitialize WiFi Config : Done"));
        }
    );
}

/**
 * @brief REinitializes relay configuration with default settings.
 */
void LFSMemory::reinitializeVarRelay(void) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &doc) {
            unsigned int id = 0;
            for (size_t i = 0; i < (sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0])); i++) {
                JsonObject data = doc.createNestedObject(
                    String(VAR_SWITCH) + String(RELAY_PINS[i]));
                data["id"] = id;
                data["name"] = VALUE_DEFAULT[i];
                data["pin"] = RELAY_PINS[i];
                data["status"] = false;

                id++;
            }

            Serial.println(F("Reinitialize Variable Relay : Done"));
        }
    );
}

/**
 * @brief Reinitializes system staste with default values.
 */
void LFSMemory::reinitializeState(void) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // Reset system state values
            this->__WATERING_MODE_STATE__ = false;
            this->__AUTO_CHANGE_MODE__    = false;

            // Update configuration data
            data[AUTOWATERING] = this->__WATERING_MODE_STATE__;
            data[AUTOCHANGE]   = this->__AUTO_CHANGE_MODE__;
            Serial.println(F("Reinitialize State : Done"));
        }
    );
}

void LFSMemory::setupLFS(void) {
    while (!LFS.begin(true)) {
        Serial.println(F("Failed... Error 0x1"));
        Serial.println(F("Error initializing LittleFS, please try again..."));
        delay(150);
    }
    
    this->createDirIfNeeded("/config");
    this->initializeWiFiConfig();
    this->initializeVarRelay();
    this->initializeState();
    this->listFiles();

    Serial.println(F("\nConfigurate WiFi client :"));
    Serial.print(F("SSID STA : "));
    Serial.println(this->__SSID_STA__);
    Serial.print(F("PASS STA : "));
    Serial.println(this->__PASS_STA__);
    Serial.println(F("\nAccess Point Server :"));
    Serial.print(F("SSID AP : "));
    Serial.println(this->__SSID_AP__);
    Serial.print(F("PASS AP : "));
    Serial.println(this->__PASS_AP__);
    delay(1000);
    Serial.println(F("Done: No error 0x0\n"));
}