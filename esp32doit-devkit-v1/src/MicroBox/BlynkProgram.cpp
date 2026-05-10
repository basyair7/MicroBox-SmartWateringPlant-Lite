/**
 *  @file BlynkProgram.cpp
 *  @version 1.0.1
 *  
 *  @author
 *  basyair7
 *  
 *  @date
 *  2026
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

#include "MicroBox/software/BlynkProgram.h"
#include "MicroBox/software/ProgramWiFi"
#include "MicroBox/software/MyEEPROM"
#include "MicroBox/externobj"

// Blynk関連のコードを実装するファイル。Blynkのセットアップ、データ送信、および仮想ピンのハンドラー関数を定義する。
#include "envBlynk.h"
#include <BlynkSimpleEsp32.h>

MyEEPROM myeeprom_obj; //!< MyEEPROMオブジェクトのインスタンス。EEPROMへの読み書きを管理するために使用される。
BlynkTimer Timer;      //!< BlynkTimerオブジェクトのインスタンス。Blynkのタイマー機能を使用して、定期的なタスクをスケジュールするために使用される。

bool switch_state; //!< Blynkの仮想ピンの状態を保持する変数。Blynkアプリからの入力を処理するために使用される。

// unsigned long _rttStart = 0, rtt = 0;
// void sendRTT(void) {
//     _rttStart = millis();
//     Blynk.virtualWrite(V5, _rttStart);
// }

// BLYNK_WRITE(V5) {
//     unsigned long now = millis();

//     if (_rttStart > 0 && now >= _rttStart) {
//         rtt = now - _rttStart;
//     }
// }


/**
 * @brief Blynkの仮想ピンV4に対するハンドラー関数。Blynkモードの無効化やWiFiモードの切り替えを管理する。
 * @details この関数は、Blynkアプリから仮想ピンV4に対してコマンドが送信されたときに呼び出される。
 *          Blynkモードを無効化し、WiFiモードをWIFI_APに切り替えるための処理を行う。
 * 
 * @param V4 仮想ピンV4の値を処理するための引数。Blynkアプリから送信された値を受け取る。
 */
// BLYNK_WRITE(V4) {
//     __lastTimeReboot__ = millis();
//     RebootState = true;
// }

/**
 * @brief Blynkの仮想ピンV3に対するハンドラー関数。手動灌漑の制御を管理する。
 * @details この関数は、Blynkアプリから仮想ピンV3に対してコマンドが送信されたときに呼び出される。
 *          手動灌漑の状態を制御し、オート灌漑が有効な場合は手動灌漑を無効にする処理を行う。
 *          - オート灌漑が有効な場合は、手動灌漑の状態をBlynkアプリに反映させる。
 *          - オート灌漑が無効な場合は、リレーを制御して手動灌漑の状態を切り替える。
 * 
 * @param V4 仮想ピンV4の値を処理するための引数。Blynkアプリから送信された値を受け取る。
 */
// BLYNK_WRITE(V4) {
//     if (wateringSys.AutoWateringState) {
//         Blynk.virtualWrite(V2, wateringSys.AutoWateringState);
//         Blynk.virtualWrite(V3, wateringSys.WateringProcess);
//     }
//     else {
//         for (const auto &pin : RELAY_PINS) {
//             RelayController::WRITE(
//                 pin, 
//                 param.asInt() == 1 ? true : false,
//                 1000
//             );
//         }
//     }
// }

/**
 * @brief Blynkの仮想ピンV3に対するハンドラー関数。手動灌漑の制御を管理する。
 * @details この関数は、Blynkアプリから仮想ピンV3に対してコマンドが送信されたときに呼び出される。
 *          手動灌漑の状態を制御し、オート灌漑が有効な場合は手動灌漑を無効にする処理を行う。
 * 
 * @param V3 仮想ピンV3の値を処理するための引数。Blynkアプリから送信された値を受け取る。
 */
// BLYNK_WRITE(V3) {
//     if (param.asInt() == 1) {
//         myeeprom_obj.save_wifi_state(false);
//         delay(50);
//         __lastTimeReboot__ = millis();
//         RebootState = true;
//     }
// }

/**
 * @brief Blynkの仮想ピンV2に対するハンドラー関数。オート灌漑の設定を管理する。
 * @details この関数は、Blynkアプリから仮想ピンV2に対してコマンドが送信されたときに呼び出される。
 * @param V2 仮想ピンV2の値を処理するための引数。Blynkアプリから送信された値を受け取る。オート灌漑の状態を切り替えるために使用される。
 *          - オート灌漑の状態をEEPROMに保存する。
 */
// BLYNK_WRITE(V2) {
//     lfsprog.changeConfigState(
//         AUTOWATERING,
//         param.asInt() == 1 ? true : false
//     );
// }

/**
 * @brief Blynkの仮想ピンV0とV1に対する定期的なデータ送信を管理する関数。土壌水分レベルとDHT温度をBlynkアプリに送信する。
 * @details この関数は、BlynkTimerを使用して定期的に呼び出される。土壌水分レベルとDHT温度をBlynkアプリの仮想ピンV0とV1に送信する。
 *          - 土壌水分レベルは仮想ピンV0に送信される。
 *          - DHT温度は仮想ピンV1に送信される。
 */
void sendDataSensor(void) {
    static unsigned long _LastMillisSendData = 0;
    if ((unsigned long) (millis() - _LastMillisSendData) >= 100) {
        _LastMillisSendData = millis();
        Blynk.virtualWrite(V1, tdsprog.getPPMValue());
        Blynk.virtualWrite(V0, soilmoisture.value);
    }
}

void sendDataState(void) {
    static unsigned long _LastMillisSendData = 0;
    if ((unsigned long) (millis() - _LastMillisSendData) >= 100) {
        _LastMillisSendData = millis();
        Blynk.virtualWrite(V2, wateringSys.WateringProcess ? "RUN" : "IDLE");
        Blynk.virtualWrite(V3, fertilizerProg.stateToString());
    }
}


/**
 * @brief Blynkを初期化し、定期タスクを設定する。
 * @details この関数は、WiFiモードがSTAの場合にBlynkを初期化し、定期的なデータ送信のためのタイマーを設定する。
 *          - Blynk.begin()を呼び出してBlynkを初期化する。
 *          - BlynkTimerを使用して、sendDataSensor関数を500ミリ秒ごとに呼び出すように設定する。
 */
void BlynkSetup() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.begin(
            BLYNK_AUTH_TOKEN,
            lfsprog.__SSID_STA__.c_str(),
            lfsprog.__PASS_STA__.c_str()
        );
        Timer.setInterval(500L, sendDataSensor);
        Timer.setInterval(500L, sendDataState);
        // Timer.setInterval(2000L, sendRTT);
    }
}

/**
 * @brief Blynkとタイマーのタスクを継続的に実行する関数。WiFiモードがSTAの場合にBlynk.run()とTimer.run()を呼び出す。
 * @details この関数は、WiFiモードがSTAの場合にBlynkとタイマーのタスクを継続的に実行するために呼び出される。Blynk.run()を呼び出してBlynkのイベント処理を行い、Timer.run()を呼び出して定期タスクを実行する。
 *          - WiFiモードがSTAでない場合は、Blynkとタイマーのタスクは実行されない。
 *          - Blynk.run()はBlynkのイベント処理を行い、Timer.run()は定期タスクを実行する。
 *          - この関数は、Blynkとタイマーのタスクを継続的に実行するために、メインループ内で呼び出されることが想定されている。
 */
void BlynkRun() {
    if (WiFi.getMode() == WIFI_STA) {
        Blynk.run();
        Timer.run();
    }
}
