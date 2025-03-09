/**
 *  @file confighandlers.cpp
 *  @version 1.0.0
 *  @date 2025
 *  @author basyair7
 *  
 *  @copyright
 *  Copyright (C) 2025, basyair7
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#include "MicroBox/software/LFSMemory"
#include "envWiFi.h"

/**
 *  changeConfigWiFi_STA
 *  @param new_ssid
 *  @param new_pass
 */
void LFSMemory::changeConfigWiFi_STA(String new_ssid, String new_pass) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            data[SSID_STA] = new_ssid;
            data[PASS_STA] = new_pass;
        }
    );
}

/**
 *  changeConfigWiFi_AP
 *  @param new_ssid
 *  @param new_pass
 */
void LFSMemory::changeConfigWiFi_AP(String new_ssid, String new_pass) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            data[SSID_AP] = new_ssid;
            data[PASS_AP] = new_pass;
        }
    );
}

/**
 *  changeConfigState
 *  @param stateConfig
 *  @param value
 */
void LFSMemory::changeConfigState(String stateConfig, bool value) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            data[stateConfig] = value;
        }
    );
}

/**
 *  readConfigState
 *  @param stateConfig
 *  @param value Pointer
 */
void LFSMemory::readConfigState(String stateConfig, bool *value) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            *value = data[stateConfig];
        }
    );
}