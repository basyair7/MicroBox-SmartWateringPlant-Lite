/**
 *  @file LFSMemory.cpp
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2026
 * 
 *  @brief このファイルは、LFSMemoryクラスの実装を含み、LittleFSを使用してWiFi設定やシステム状態の管理を行う機能を提供する。
 *  @details LFSMemoryクラスは、WiFiのSSIDやパスワードの保存、システム状態の管理など、LittleFSを活用した設定管理機能を提供します。
 *  
 *  @copyright
 *  Copyright (C) 2026, basyair7
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
 * @brief セットップ時に検証のため、LittleFSに保存されているすべてのファイルを一覧表示する。
 * @details LittleFSに保存されているファイルを一覧表示することで、設定ファイルの存在や内容を確認し、デバッグや検証に役立てることができます。
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
 * @brief 指定されたファイルに保存された WiFi 設定を初期化または更新する。
 * @details この関数は、指定されたファイルに保存された WiFi 設定を初期化または更新するための共通のロジックを提供します。
 *          ファイルが存在しない場合や内容が破損している場合は、デフォルトの WiFi 設定で初期化されます。
 *          更新は、提供されたラムダ関数を使用して行われます。
 * 
 * @param cfile WiFi 設定を保存するファイルの名前。
 * @param updateFunc WiFi 設定を更新するためのラムダ関数。StaticJsonDocument を引数に取り、必要な変更を加えることができます。
 */
void LFSMemory::initializeOrUpdateWiFiConfig(const String &cfile, std::function<void (StaticJsonDocument<500>&)> updateFunc)
{
    StaticJsonDocument<500> data;
    String __readConfig__ = this->readconfig(cfile), __newConfig__ = "";
    if (__readConfig__ == "null" || !lfsIsExists(cfile)) {
        // 設定ファイルが保存しない、または破損している場合、デフォルトの WiFi 認証情報で初期化する。
        Serial.println(F("WiFi config file missing, creating new one."));
        data[SSID_STA] = WIFI_SSID_STA_DEFAULT;
        data[PASS_STA] = WIFI_PASS_STA_DEFAULT;
        data[SSID_AP]  = WIFI_SSID_AP_DEFAULT;
        data[PASS_AP]  = WIFI_PASS_AP_DEFAULT;
    }
    else {
        DeserializationError error = deserializeJson(data, __readConfig__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateWiFiConfig", // エラー追跡用関数名
                error.c_str() // エラーメッセージ
            );
            return;
        }
    }

    // 提供されたラムダ関数を使用して変更を適用する。
    updateFunc(data);

    // 更新されたデータをシリアル化してファイルに書き込む。
    serializeJson(data, __newConfig__);
    this->writeconfig(cfile, __newConfig__);
}

/**
 * @brief 指定されたファイルに保存されたリレー設定を初期化または更新する。
 * @details この関数は、指定されたファイルに保存されたリレー設定を初期化または更新するための共通のロジックを提供します。
 * 
 * @param cfile リレー設定を保存するファイルの名前。
 * @param updateFunc リレー設定を更新するためのラムダ関数。DynamicJsonDocument を引数に取り、必要な変更を加えることができます。
 */
void LFSMemory::initializeOrUpdateVarRelay(const String &cfile, std::function<void (DynamicJsonDocument&)> updateFunc)
{
    DynamicJsonDocument doc(1024);
    String __readConfig__ = this->readconfig(cfile), __newConfig__ = "";
    
    if (__readConfig__ == "null" || !lfsIsExists(cfile)) {
        Serial.println(F("Relay config file missing, creating new one."));
        this->__initialize_data_relay__(doc);
    }
    else {
        DeserializationError error = deserializeJson(doc, __readConfig__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateVarRelay", // エラー追跡用関数名
                error.c_str() // エラーメッセージ
            );
            return;
        }
    }

    // 提供されたラムダ関数を使用して変更を適用する。
    updateFunc(doc);

    // 更新されたデータをシリアル化してファイルに書き込む。
    serializeJson(doc, __newConfig__);
    this->writeconfig(cfile, __newConfig__);
}

/**
 * @brief 指定されたファイルに保存されたシステム状態設定を初期化または更新する。
 * @details ファイルが存在しない、または破損している場合は、デフォルトの状態値が適用されます。
 * 
 * @param cfile システム状態設定を保存するファイルの名前。
 * @param updateFunc ファイル内の設定データを更新するためのラムダ関数。
 */
void LFSMemory::initializeOrUpdateState(const String &cfile, std::function<void (StaticJsonDocument<200>&)> updateFunc)
{
    StaticJsonDocument<200> doc;
    String __readConfig__ = this->readconfig(cfile), __newConfig__ = "";
    
    if (__readConfig__ == "null" || !lfsIsExists(cfile)) {
        // 状態設定ファイルが存在しない、または破損している場合、デフォルトの状態値で初期化する。
        Serial.println(F("State config file missing, creating new one."));
        doc[AUTOWATERING] = false;
        doc[AUTOCHANGE]   = false;
    }
    else {
        DeserializationError error = deserializeJson(doc, __readConfig__);
        if (error) {
            this->handleError_deserializeJson(
                "initializeOrUpdateState", // エラー追跡用関数名
                error.c_str() // エラーメッセージ
            );
            return;
        }
    }

    // 提供されたラムダ関数を使用して変更を適用する。
    updateFunc(doc);

    // 更新されたデータをシリアル化してファイルに書き込む。
    serializeJson(doc, __newConfig__);
    this->writeconfig(cfile, __newConfig__);
}

/**
 * @brief LittleFSに保存されたWiFi設定を使用して、WiFi関連の変数を初期化する。
 * @details この関数は、LittleFSに保存されたWiFi設定を読み取り、WiFi関連の変数を初期化します。
 *          設定ファイルが存在しない場合や内容が破損している場合は、デフォルトのWiFi設定で初期化されます。
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
 * @brief LittleFSに保存されたリレー設定を使用して、リレー関連の変数を初期化する。
 * @details この関数は、LittleFSに保存されたリレー設定を読み取り、リレー関連の変数を初期化します。
 *          設定ファイルが存在しない場合や内容が破損している場合は、デフォルトのリレー設定で初期化されます。
 */
void LFSMemory::initializeVarRelay(void) {
    this->initializeOrUpdateVarRelay(
        this->file_config_relay,
        [&](DynamicJsonDocument &data) {
            // リレー設定を初期化するためのコードをここに記述します。
            // 例: this->__RELAY1_STATE__ = data["relay1"].as<bool>();
        }
    );
}

/**
 * @brief LittleFSに保存されたシステム状態設定を使用して、状態関連の変数を初期化する。
 * @details この関数は、LittleFSに保存されたシステム状態設定を読み取り、状態関連の変数を初期化します。
 *          設定ファイルが存在しない場合や内容が破損している場合は、デフォルトの状態値で初期化されます。
 */
void LFSMemory::initializeState(void) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // 状態設定を初期化するためのコードをここに記述します。
            this->__WATERING_MODE_STATE__ = data[AUTOWATERING];
            this->__AUTO_CHANGE_MODE__    = data[AUTOCHANGE];
        }
    );
}

/**
 * @brief LittleFSに保存されたWiFi設定をデフォルトの値で再初期化する。
 * @details この関数は、LittleFSに保存されたWiFi設定をデフォルトの値で再初期化します。これにより、WiFi設定がリセットされ、デフォルトのSSIDとパスワードが適用されます。
 *          既存のWiFi設定ファイルが存在する場合は、上書きされます。
 */
void LFSMemory::reinitializeWiFiConfig(void) {
    this->initializeOrUpdateWiFiConfig(
        this->file_config_wifi,
        [&](StaticJsonDocument<500> &data) {
            // WiFi設定をデフォルト値にリセットする。
            this->__SSID_STA__  = WIFI_SSID_STA_DEFAULT;
            this->__SSID_AP__   = WIFI_SSID_AP_DEFAULT;
            this->__PASS_STA__  = WIFI_PASS_STA_DEFAULT;
            this->__PASS_AP__   = WIFI_PASS_AP_DEFAULT;

            // デフォルト値をデータに設定する。
            data[SSID_STA] = this->__SSID_STA__;
            data[SSID_AP]  = this->__SSID_AP__;
            data[PASS_STA] = this->__PASS_STA__;
            data[PASS_AP]  = this->__PASS_AP__;

            Serial.println(F("Reinitialize WiFi Config : Done"));
        }
    );
}

/**
 * @brief LittleFSに保存されたリレー設定をデフォルトの値で再初期化する。
 * @details この関数は、LittleFSに保存されたリレー設定をデフォルトの値で再初期化します。これにより、リレー設定がリセットされ、デフォルトのリレー状態が適用されます。
 *          既存のリレー設定ファイルが存在する場合は、上書きされます。
 */
void LFSMemory::reinitializeVarRelay(void) {
    DynamicJsonDocument doc(1024);

    this->__initialize_data_relay__(doc);

    Serial.println(F("Reinitialize Variable Relay : Writing new file"));

    String __newConfig__;
    serializeJson(doc, __newConfig__);
    this->writeconfig(this->file_config_relay, __newConfig__);
}

/**
 * @brief LittleFSに保存されたシステム状態設定をデフォルトの値で再初期化する。
 * @details この関数は、LittleFSに保存されたシステム状態設定をデフォルトの値で再初期化します。これにより、システム状態がリセットされ、デフォルトの状態値が適用されます。
 *          既存の状態設定ファイルが存在する場合は、上書きされます。
 */
void LFSMemory::reinitializeState(void) {
    this->initializeOrUpdateState(
        this->file_config_state,
        [&](StaticJsonDocument<200> &data) {
            // 状態設定をデフォルト値にリセットする。
            this->__WATERING_MODE_STATE__ = false;
            this->__AUTO_CHANGE_MODE__    = false;

            // デフォルト値をデータに設定する。
            data[AUTOWATERING] = this->__WATERING_MODE_STATE__;
            data[AUTOCHANGE]   = this->__AUTO_CHANGE_MODE__;
            Serial.println(F("Reinitialize State : Done"));
        }
    );
}

/**
 * @brief LittleFSに保存されたWiFi設定を変更するための設定ハンドラを提供する。
 * @details この関数は、WiFiステーションモードのSSIDとパスワードを変更するための設定ハンドラです。提供された新しいSSIDとパスワードを使用して、WiFi設定を更新します。
 *          更新された設定は、LittleFSに保存されます。
 */
void LFSMemory::setupLFS(void) {
    // LittleFSの初期化を試みる。失敗した場合は、エラーメッセージを表示して再試行する。
    while (!LFS.begin(true)) {
        Serial.println(F("Failed... Error 0x1"));
        Serial.println(F("Error initializing LittleFS, please try again..."));
        delay(150);
    }
    
    // LittleFSの初期化に成功した場合、必要なディレクトリを作成し、設定ファイルを初期化して、保存されているファイルを一覧表示する。
    this->createDirIfNeeded("/config");
    this->initializeWiFiConfig();
    this->initializeVarRelay();
    this->initializeState();
    this->listFiles();

    // LittleFSに保存されたWiFi設定をシリアルモニタに表示する。
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