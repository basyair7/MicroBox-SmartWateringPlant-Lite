/**
 *  @file LFSProgram.cc
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

#include "MicroBox/LFSProgram.h"

/**
 * List all files store in LittleFS, grouped by categories.
 * This function help in verifying stored files during setup.
 */
void LFSProgram::listFiles(void) {
    Serial.println(F("Listing file stored in LittleFS : "));
    this->PrintList("/");
    this->PrintList("/CONFIG");
    this->PrintList("/WEB");
    this->PrintList("/WEB/html");
    this->PrintList("/WEB/css");
    this->PrintList("/WEB/js");
}

/**
 * Initializes or updates WiFi configuration settings stored in a given file.
 * If the file is missing or corrupted, default WiFi credentials will be used.
 * 
 * @param cfile: Configuration file name containing WiFi settings.
 * @param updateFunc: Lambda function to update configuration data in the file.
 */
void LFSProgram::initializeOrUpdateWiFiConfig(const String &cfile, std::function<void (StaticJsonDocument<500>&)> updateFunc)
{
    StaticJsonDocument<500> data;
    String __readConfig__ = this->readconfig(cfile), __newConfig__ = "";
    if (__readConfig__ == "null" || !lfsIsExists(cfile)) {
        // Initialize with default WiFi credentials if the config is missing or corrupted.
        data["SSID_STA"] = WIFI_SSID_STA_DEFAULT;
        data["PSK_STA"]  = WIFI_PSK_STA_DEFAULT;
        data["SSID_AP"]  = WIFI_SSID_AP_DEFAULT;
        data["PSK_AP"]   = WIFI_PSK_AP_DEFAULT;
    }
    else {
        DeserializationError error = deserializeJson(data, __readConfig__);
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
 * Initializes or update relay configuration in the specified file.
 * If the is missing or corrupted, default relay settings will be used.
 * 
 * @param cfile: Configuration file name containing relay settings.
 * @param updateFunc: Lambda function to update configuration data in the file.
 */
void LFSProgram::initializeOrUpdateVarRelay(const String &cfile, std::function<void (DynamicJsonDocument&)> updateFunc)
{
    DynamicJsonDocument doc(500);
    String __readconf__ = this->readconfig(cfile), __newconf__ = "";

    if (__readconf__ == "null" || !lfsIsExists(cfile)) {
        // Initialize relay data with default values
        this->__initialize_data_relay__(doc);
    }
    else {
        DeserializationError error = deserializeJson(doc, __readconf__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateVarRelay", 
                error.c_str()
            );
            return;
        }
    }

    // Apply the changes using the provided lambda function
    updateFunc(doc);

    // Serialize updated data and write it back to the file
    serializeJson(doc, __newconf__);
    this->writeconfig(cfile, __newconf__);
}

/**
 * Initializes or updates system state settings in the specified configuration file.
 * Default state values are applied if the file is missing or corrupted.
 * 
 * @param cfile: Configuration file name containing state settings.
 * @param updateFunc: Lambda function to update configuration data in the file.
 */
void LFSProgram::initializeOrUpdateState(const String &cfile, std::function<void (StaticJsonDocument<200>&)> updateFunc)
{
    StaticJsonDocument<200> __data__;
    String __readconf__ = this->readconfig(cfile), __newconf__ = "";
    if (__readconf__ == "null" || !lfsIsExists(cfile)) {
        // Initialize with default state values if the config is missing or corrupted
        __data__["AUTO_WATERING"] = true;
        __data__["AUTO_CHANGE"] = true;
    }
    else {
        DeserializationError error = deserializeJson(__data__, __readconf__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateState",  // Function name for error tracking
                error.c_str()   // Error message
            );
            return;
        }
    }

    // Apply the changes using the provided lambda function
    updateFunc(__data__);

    // Serialize updated data and write it back to the file
    serializeJson(__data__, __newconf__);
    this->writeconfig(cfile, __newconf__);
}

/**
 * Initializes WiFi configuration with values from the configuration file.
 */
void LFSProgram::initializeWiFiConfig(void) {
    this->initializeOrUpdateWiFiConfig(
        file_config_wifi, 
        [&](StaticJsonDocument<500> &data) {
            this->__SSID_AP__      = data["SSID_AP"].as<String>();
            this->__SSID_STA__     = data["SSID_STA"].as<String>();
            this->__PASSWORD_AP__  = data["PSK_AP"].as<String>();
            this->__PASSWORD_STA__ = data["PSK_STA"].as<String>();
        }
    );
}

/**
 * Initializes relay configuration using the stored data.
 */
void LFSProgram::initializeVarRelay(void) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            // Placeholder for relay configuration updates
        }
    );
}

/**
 * Initializes system state settings from the configuration file.
 */
void LFSProgram::initializeState(void) {
    this->initializeOrUpdateState(
        file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // Extract system state values from the configuration
            this->__WATERING_MODE_STATE__ = data["AUTO_WATERING"];
            this->__AUTO_CHANGE_MODE__ = data["AUTO_CHANGE"];
        }
    );
}

/**
 * Reinitializes WiFi configuration with default values.
 */
void LFSProgram::reinitializeWiFiConfig(void) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            // Reset to default WiFi credentials
            this->__SSID_AP__      = WIFI_SSID_AP_DEFAULT;
            this->__SSID_STA__     = WIFI_SSID_STA_DEFAULT;
            this->__PASSWORD_AP__  = WIFI_PSK_AP_DEFAULT;
            this->__PASSWORD_STA__ = WIFI_PSK_STA_DEFAULT;

            // Update configuration data
            data["SSID_AP"]  = this->__SSID_AP__;
            data["SSID_STA"] = this->__SSID_STA__;
            data["PSK_AP"]   = this->__PASSWORD_AP__;
            data["PSK_STA"]  = this->__PASSWORD_STA__;

            Serial.println(F("Reinitialize WiFi Config : Done"));
        }
    );
}

/**
 * Reinitializes relay configuration with default settings.
 */
void LFSProgram::reinitializeVarRelay(void) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &doc) {
            unsigned int id = 0;
            for (int i = 0; i < (int)(sizeof(RELAY_PINS)/sizeof(RELAY_PINS[0])); i++) {
                JsonObject data = doc.createNestedObject(PIN_IO_ALIAS[RELAY_PINS[i]]);
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
 * Reinitializes system state with default values.
 */
void LFSProgram::reinitializeState(void) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // Reset system state values
            this->__WATERING_MODE_STATE__ = true;
            this->__AUTO_CHANGE_MODE__ = false;

            // Update configuration data
            data["AUTO_WATERING"] = this->__WATERING_MODE_STATE__;
            data["AUTO_CHANGE"] = this->__AUTO_CHANGE_MODE__;
            Serial.println(F("Reinitialize State : Done"));
        }
    );
}

void LFSProgram::setupLFS(void) {
    while (!LFS.begin()) {
        Serial.println(F("Failed..."));
        Serial.println(F("Error initializing LittleFS, please try again..."));
        Serial.println(F("Done : Error 0x1"));
        delay(150);
    }

    this->initializeWiFiConfig();
    this->initializeState();
    this->initializeVarRelay();
    this->listFiles();
    
    Serial.println(F("\nConfiguration WiFi Client : "));
    Serial.print(F("SSID STA : ")); Serial.println(this->__SSID_STA__);
    Serial.print(F("PSK STA : ")); Serial.println(this->__PASSWORD_STA__);
    Serial.println(F("\nAccess Point Server : "));
    Serial.print(F("SSID AP : ")); Serial.println(this->__SSID_AP__);
    Serial.print(F("PSK AP : ")); Serial.println(this->__PASSWORD_AP__);
    delay(1000);
    Serial.println(F("Done : No error 0x0\n"));
}
