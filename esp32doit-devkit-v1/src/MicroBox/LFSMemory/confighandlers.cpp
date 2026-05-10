/**
 *  @file confighandlers.cpp
 *  @version 1.0.1
 *  @brief このファイルは、LFSMemoryの設定バンドラの実装を含み、WiFiの設定を変更する機能や、
 *         状態設定の読み書き機能を提供する。
 * 
 *  @date 2026
 *  @author basyair7
 *  
 *  @copyright
 *  Copyright (C) 2026, basyair7
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
 *  @brief LFSMemory::changeConfigWiFi_STA
 *  @details この関数は、WiFiステーションモードのSSIDとパスワードを変更するための設定ハンドラです。
 *  @param new_ssid 新しいSSID
 *  @param new_pass 新しいパスワード
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
 *  @brief LFSMemory::changeConfigWiFi_AP
 *  @details この関数は、WiFiアクセスポイントモードのSSIDとパスワードを変更するための設定ハンドラです。
 *  @param new_ssid 新しいSSID
 *  @param new_pass 新しいパスワード
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
 *  @brief LFSMemory::changeConfigState
 *  @details この関数は、指定された状態設定を変更するための設定ハンドラです。
 *  
 *  @param stateConfig 変更する状態設定のキー
 *  @param value 新しい値
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
 * 
 */
void LFSMemory::changeFertilizerConfigTime(uint8_t _hour, uint8_t _minute, uint32_t _second, uint32_t _interval)
{
    this->initializeOrUpdateFertilizerConfig(
        this->file_config_fertilizer,
        [&](StaticJsonDocument<200> &data) {
            data["hour"]     = _hour;
            data["minute"]   = _minute;
            data["second"]   = _second;
            data["interval"] = _interval;
        }
    );
}

void LFSMemory::changeFertilizerConfig(int _ppm_target, int _ppm_tolerance)
{
    this->initializeOrUpdateFertilizerConfig(
        this->file_config_fertilizer,
        [&](StaticJsonDocument<200> &data) {
            data["ppm_target"] = _ppm_target;
            data["ppm_tolerance"] = _ppm_tolerance;
        }
    );
}

/**
 *  @brief LFSMemory::readConfigState
 *  @details この関数は、指定された状態設定を読み取るための設定ハンドラです。
 *  @param stateConfig 読み取る状態設定のキー
 *  @param value 読み取った値を格納するためのポインタ
 */
void LFSMemory::readConfigState(String stateConfig, bool *value) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            *value = data[stateConfig];
        }
    );
}