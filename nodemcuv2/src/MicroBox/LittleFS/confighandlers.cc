/**
 *  @file confighandlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/LFSProgram.h"
#include "envWiFi.h"

/*! changeConfigWiFi_STA
 * @param new_ssid_sta
 * @param new_psk_sta
*/
void LFSProgram::changeConfigWiFi_STA(String new_ssid_sta, String new_psk_sta)
{
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            data["SSID_STA"] = new_ssid_sta;
            data["PSK_STA"] = new_psk_sta;
        }
    );
}

/*! changeConfigWiFi_AP
 * @param new_ssid_ap
 * @param new_psk_ap
*/
void LFSProgram::changeConfigWiFi_AP(String new_ssid_ap, String new_psk_ap)
{
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            data["SSID_AP"] = new_ssid_ap;
            data["PSK_AP"] = new_psk_ap;
        }
    );
}

/*! changeConfigState
 * @param stateConfig
 * @param value
*/
void LFSProgram::changeConfigState(String stateConfig, bool value) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            data[stateConfig] = value;
        }
    );
}

/*! readConfigState
 * @param stateConfig
 * @param value
*/
void LFSProgram::readConfigState(String stateConfig, bool *value) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            *value = data[stateConfig];
        }
    );
}
