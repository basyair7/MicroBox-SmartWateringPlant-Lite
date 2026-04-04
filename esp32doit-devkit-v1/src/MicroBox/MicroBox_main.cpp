/**
 *  @file MicroBox_main.cpp
 *  @version 1.0.1
 *  @brief MicroBoxプロジェクトのメイプログラムファイル。本ファイルでは、setup関数およびloop関数の処理を行う。
 *  @details このファイルは、MicroBoxプロジェクトのメインプログラムファイルであり、ESP32マイクロコントローラーを使用して、
 *           センサーの読み取り、WiFi接続、Blynkアプリケーションとの通信、およびその他のシステム機能を管理します。
 *           FreeRTOSを使用して複数のタスクを実行し、センサーの更新、Blynkの更新、およびシステム情報の更新を効率的に処理します。
 *  
 *  @author
 *  basyair7
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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "MicroBox_main.h"
#include "ThisRTOS.hpp"

// センサーとモジュールのヘッダーをインクルード
#include "MicroBox/hardware/BootButton.h"
#include "MicroBox/hardware/LEDBoard.h"
#include "MicroBox/hardware/sensor/DHTProgram"
#include "MicroBox/hardware/sensor/SoilMoisture"
#include "MicroBox/hardware/sensor/TDSProgram"
#include "MicroBox/hardware/sensor/WaterTemp"
#include "MicroBox/hardware/RelayController"
#include "MicroBox/hardware/LCDdisplay"
#include "MicroBox/hardware/DS3231rtc"

// システムヘッダーをインクルード
#include "MicroBox/software/BlynkProgram.h"
#include "MicroBox/software/ButtonManager"
#include "MicroBox/software/MyEEPROM"
#include "MicroBox/software/LFSMemory"
#include "MicroBox/software/SysHandlers"
#include "MicroBox/software/ProgramWiFi"
#include "MicroBox/software/WebServer"
#include "MicroBox/software/WateringSys"
#include "MicroBox/software/FertilizerProgram"
#include "MicroBox/config.h"
#include "MicroBox/externobj"

#include "variable"
#include "envWiFi.h"

// モジュールとグローバル変数を初期化
// センサープログラムを初期化
SoilMoisture soilmoisture; //!< 土壌水分センサー管理モジュール

// TDSセンサープログラム
const TDSConfig tds_config = {
    PIN_TDS,
    VREF,
    ADC_RESOLUTION,
    TDS_A,
    TDS_B,
    TDS_VOLTAGE_OFFSET,
    SCOUNT
};
TDSProgram tdsprog = TDSProgram(tds_config);    //!< TDSセンサー管理モジュール

WaterTemp watertemp = WaterTemp(PIN_WATERTEMP); //!< 水温センサー管理モジュール

// ハードウェアコンポーネントを初期化
BootButton bootbtn = BootButton(BOOTBUTTON, INPUT); //!< ブートボタンユーティリティ
LEDBoard led_running, led_warning; //!< LEDインジケータープログラム
DS3231rtc rtcprog; //!< 時間管理のためのRTCプログラム
LCDdisplay lcd = LCDdisplay(); //!< LCDユーティリティモジュール
DHTProgram dhtprog = DHTProgram(PIN_DHT, DHT22); //!< DHTセンサープログラム
// RelayController relayController; //!< リレーマネジメントモジュール

// システムプログラムを初期化
MyEEPROM myeeprom_prog;  //!< EEPROMユーティリティモジュール
LFSMemory lfsprog;       //!< LittleFSマネジメントモジュール
WateringSys wateringSys; //!< 灌水システムプログラム
FertilizerProgram fertilizerProg; //<! 肥料システムプログラム

// タスク実行のためのミリ秒トラッカー
unsigned long __lastMillis__ = 0, __lastTimeReboot__ = 0;
bool RebootState = false; //!< Tracks ESP reboot state
bool otaDisplay = false;

/**
 * @brief I2C通信用のミューテックス
 * @details 複数タスクからI2Cデバイス（LCDやRTCなど）へ同時アクセスする際の競合を防ぐために使用する。
 *          I2C通信を行う前にロックし、完了後に解放することで排他制御を実現する。
 */
SemaphoreHandle_t i2cMutex;

// LCD表示モード
enum LCDMode {
    LCD_AUTO,
    LCD_MANUAL
};

/**
 * @brief センサー更新と灌水制御を実行するタスク。
 * @param pvParameter タスクのパラメータ（使用されない）。
 * @details このタスクは、センサーデータ（土壌水分とDHT）を継続的に読み取り、
 *          センサー読み取り値と定義済みのロジックに基づいて灌水システムを実行します。
 *          また、デバッグと監視のためにシリアルモニターにセンサー値を印刷します。
 */
void ThisRTOS::vTask1(void *pvParameter) {
    (void) pvParameter; // Unused parameter

    // Initialize sensor
    soilmoisture.begin(PIN_SMS);
    dhtprog.begin();
    tdsprog.begin();
    watertemp.begin();

    // pvParameterからMicroBox_Mainインスタンスのポインタを取得
    MicroBox_Main* mainObj = static_cast<MicroBox_Main*>(pvParameter);

    while (true) {
        // 土壌水分センサーを実行し、読み取り値を更新
        soilmoisture.getData(true, 3395, 4095);

        // DHTセンサーを実行し、読み取り値を更新
        dhtprog.running();

        // 水温センサーを実行し、読み取り値を更新
        watertemp.update();

        // TDSプログラムに水温を設定して補正
        tdsprog.setTemperature(watertemp.getTemperature());

        // TDSセンサーを実行し、読み取り値を更新
        tdsprog.update();

        // DisplayProgram関数を呼び出してLCD表示を更新
        mainObj->DisplayProgram();

        // タスク実行頻度を制御するために100ミリ秒遅延
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Blynk更新とWebサーバーOTA更新を実行するタスク。
 * @param pvParameter タスクのパラメータ（使用されない）。
 * @details このタスクは、Blynk更新関数を継続的に実行し、
 *          WiFiモードがアクセスポイント（AP）に設定されている場合、
 *          オーバー・ザ・エア（OTA）更新のためにWebサーバーも更新します。
 *          これにより、Blynkアプリケーションの応答性が維持され、
 *          必要に応じてWebサーバーがOTA更新を処理できます。
 */
void ThisRTOS::vTask2(void *pvParameter) {
    (void) pvParameter; // Unused parameter

    // WiFi状態を読み取り、初期化
    bool wifi_state = myeeprom_prog.read(ADDR_EEPROM_WIFI_MODE);
    // 自動変更WiFiを読み取り
    bool _autoChangeState;
    lfsprog.readConfigState(AUTOCHANGE, &_autoChangeState);
    
    Serial.print(F("Auto Change WiFi MODE: "));
    Serial.println(_autoChangeState ? "Enable" : "Disable");

    // WiFiプログラムを初期化
    ProgramWiFi.setup(
        lfsprog.__SSID_STA__, lfsprog.__PASS_STA__,
        lfsprog.__SSID_AP__, lfsprog.__PASS_AP__
    );
    ProgramWiFi.initWiFi(wifi_state);

    // Blynkプログラムを初期化
    BlynkSetup();

    // WebServerプログラムを初期化
    if (WiFi.getMode() == WIFI_AP)
        WebServer.ServerInit();

    while (true) {
        // Blynkアプリケーションの応答性を保つためにBlynk更新関数を実行
        BlynkRun();

        // WiFiモードがアクセスポイント（AP）に設定されている場合、OTA更新のためにWebを更新
        if (WiFi.getMode() == WIFI_AP) {
            WebServer.UpdateOTAloop();
        }

        // タスク実行頻度を制御するために100ミリ秒遅延
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief システム情報更新と再起動処理を実行するタスク。
 * @param pvParameter タスクのパラメータ（使用されない）。
 */
void ThisRTOS::vTask3(void *pvParameter) {
    (void) pvParameter;
    
    wateringSys.begin();
    fertilizerProg.begin(RELAY_FERTILIZER, RELAY_MOTOR);
    ButtonManager.init();
    lcd.backlight(ButtonManager.backlightState);

    while (true) {
        // 必要に応じてシステム再起動ロジックを実行
        RebootSys::run(&__lastTimeReboot__, RebootState);

        // 自動状態変更ロジックを実行
        bootbtn.ChangeWiFiMode();
        AutoChangeState::run();

        ButtonManager.update();

        // RelayControllerのプロセスキューを実行
        RelayController::PROCESSQUEUE();

        if (WiFi.status() == WL_DISCONNECTED || WiFi.getMode() == WIFI_AP) 
            led_warning.run(1500);

        wateringSys.run();
        fertilizerProg.run(HOUR_FERTILIZER, MINUTE_FERTILIZER);

        // タスク実行頻度を制御するために100ミリ秒遅延
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief ハードウェアとモジュールを初期化するセットアップ関数。
 * @param baud シリアル通信のボーレート。
 * @details この関数は起動時に一度実行されます。以下を初期化します：
 *          - シリアル通信とEEPROM
 *          - WiFi、Webサーバー、Blynk（STAモードの場合）
 *          - RTOSプログラム
 */
void MicroBox_Main::setup(unsigned long baud) {
    Wire.begin(21, 22); // SDA, SCL
    Wire.setClock(100000);
    Wire.setTimeOut(200);

    Serial.begin(baud); //!< Initialize serial communication

    // EEPROMを初期化
    myeeprom_prog.initialize();

    // LittleFSプログラムを初期化
    lfsprog.setupLFS();

    // ハードウェアコンポーネントを初期化
    i2cMutex = xSemaphoreCreateMutex();
    xSemaphoreTake(i2cMutex, portMAX_DELAY);
    lcd.init();
    rtcprog.begin(); //!< RTCプログラムを初期化
    xSemaphoreGive(i2cMutex);
    
    RelayController::BEGIN();
    led_running.begin(LED_RUNNING);
    led_warning.begin(LED_WARNING);
    bootbtn.begin();

    this->splash_boot(1000);
    
    // FreeRTOSタスクを作成
    static ThisRTOS rtos;
    // タスクを作成し、vTask 1を実行
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask1(param);
    }, "Task 1", 4096, &rtos, 1, NULL, PRO_CPU_NUM);
    
    // タスクを作成し、vTask 2を実行
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask2(param);
    }, "Task 2", 4096, &rtos, 1, NULL, APP_CPU_NUM);

    // タスクを作成し、vTask 3を実行
    xTaskCreateUniversal([](void *param) {
        static_cast<ThisRTOS*>(param)->vTask3(param);
    }, "Task 3", 4096, &rtos, 1, NULL, APP_CPU_NUM);

}

void MicroBox_Main::loop() {
    /** 
     * ループには、他のタスクや
     * 実行する必要のある機能を含めることができます
    */
}

/**
 * @brief 起動時のスプラッシュ画面を表示する
 * @details プロジェクト名（NAMEPROJECT）をLCD上で左スクロール表示し、
 *          その後にバージョン情報（VERSIONPROJECT）を表示する。
 *          一定時間表示した後、画面をクリアする。
 * 
 * @param _delay 表示後に待機する時間（ミリ秒）※現在は未使用
 */
void MicroBox_Main::splash_boot(uint32_t _delay) {
    LiquidCrystal_animated running_text;
    size_t len = strlen(NAMEPROJECT);

    // バージョン情報を表示する。
    lcd.print("Version : ", 0, 1);
    lcd.print(SWVERSION);

    // プロジェクト名を左スクロール表示する。
    for (byte i = 0; i < len + 16; i++) {
        lcd.print(running_text.Scroll_LCD_Left(NAMEPROJECT), 0, 0);
        delay(150);
    }
    running_text.Clear_Scroll_LCD_Left();

    // 表示後に画面をクリアする。
    delay(_delay);
    lcd.clear();
}

/**
 * @brief LCD表示のメイン制御処理
 * @details 各種センサー情報およびシステム状態をLCDに表示する。
 *          表示モードは自動（スライド切替）と手動（ボタン操作）をサポートする。
 * 
 *          - 自動モード（LCD_AUTO）：
 *              一定時間ごとに表示内容を順番に切り替える。
 *          - 手動モード（LCD_MANUAL）：
 *              ボタン操作により表示内容を切り替える。
 * 
 *          表示内容には以下が含まれる：
 *          - 水温およびTDS値
 *          - 土壌水分
 *          - 温度・湿度（DHT）
 *          - 自動灌水状態および現在の灌水状態
 *          - 肥料システムの状態およびスケジュール
 *          - WiFiモードおよび接続状態
 *          - RTCによる日時情報
 * 
 *          また、OTA表示中（otaDisplay == true）の場合は、
 *          通常のLCD更新を停止し、OTA表示を優先する。
 * 
 *          RTCデータ取得およびLCD描画時には、
 *          I2Cミューテックスを使用して排他制御を行う。
 */
constexpr uint8_t SLIDE_DURATION = 5;
constexpr uint8_t TOTAL_SLIDES = 12;
constexpr uint8_t MAX_STATE = SLIDE_DURATION * TOTAL_SLIDES;
constexpr uint8_t AUTO_MODE_INDEX = TOTAL_SLIDES;
void MicroBox_Main::DisplayProgram() {
    bool watering_process = wateringSys.WateringProcess;
    watering_process ? led_running.on() : led_running.off();
    
    uint8_t btnSlide = ButtonManager.updateDisplay(AUTO_MODE_INDEX);

    static String rtcDate;
    static String rtcTime;
    static unsigned long lastRTC = 0;

    if (millis() - lastRTC >= 1000) {

        if (xSemaphoreTake(i2cMutex, portMAX_DELAY)) {
            rtcDate = rtcprog.datestr();
            rtcTime = rtcprog.timestr();
            xSemaphoreGive(i2cMutex);
        }

        lastRTC = millis();
    }

    static unsigned long LastTimeRefreshLCD = 0;
    if ((unsigned long) (millis() - LastTimeRefreshLCD) >= 1000L) {
        LastTimeRefreshLCD = millis();

        static LCDMode lcdMode = LCD_AUTO;
        static int lcdState = 0;
        static uint8_t autoSlide = 0;
        
        uint8_t slide;

        if (btnSlide == AUTO_MODE_INDEX) {
            lcdMode = LCD_AUTO;
        } else {
            lcdMode = LCD_MANUAL;
        }

        if (lcdMode == LCD_AUTO) {
            lcdState = (lcdState + 1) % MAX_STATE;
            slide = lcdState / SLIDE_DURATION;
        }
        else {
            slide = btnSlide;
        }

        if (!otaDisplay && xSemaphoreTake(i2cMutex, portMAX_DELAY)) {
            lcd.clear();
            switch (slide) {
                case 0:
                {
                    lcd.print("Temp:", 0, 0);
                    lcd.print(watertemp.getTemperature());
                    lcd.print("C");

                    lcd.print("TDS:", 0, 1);
                    lcd.print(tdsprog.getTDSValue());
                    lcd.print("ppm");
                }
                break;

                case 1:
                {
                    lcd.print("Soil Moisture", 0, 0);
                    lcd.print("Value:", 0, 1);
                    lcd.print(soilmoisture.value);
                    lcd.print("%");
                }
                break;

                case 2:
                {
                    lcd.print("Temp:", 0, 0);
                    lcd.print(dhtprog.temperature);
                    lcd.print("C");

                    lcd.print("Hum:", 0, 1);
                    lcd.print(dhtprog.humidity);
                    lcd.print("%");
                }
                break;

                case 3:
                {
                    lcd.print("Auto Watering:", 0, 0);
                    lcd.print(wateringSys.AutoWateringState ? "Enable" : "Disable", 0, 1);
                }
                break;

                case 4:
                {
                    lcd.print("Watering:", 0, 0);
                    lcd.print(watering_process ? "RUN" : "IDLE", 0, 1);
                }
                break;

                case 5:
                {
                    lcd.print("Fertilizer:", 0, 0);
                    lcd.print(fertilizerProg.stateToString(), 0, 1);
                }
                break;

                case 6:
                {
                    lcd.print("Fertilizer Date:", 0, 0);
                    lcd.print("Next:", 0, 1);
                    lcd.print(fertilizerProg.getNextFertilizerDay());
                }
                break;

                case 7:
                {
                    lcd.print("Fertilizer Date:", 0, 0);
                    lcd.print("Passed:", 0, 1);
                    lcd.print(fertilizerProg.getDaysPassed());
                
                }
                break;

                case 8:
                {
                    lcd.print("Fertilizer Date:", 0, 0);
                    lcd.print("Remaining:", 0, 1);
                    lcd.print(fertilizerProg.getDaysRemaining());
                }
                break;

                case 9:
                {
                    lcd.print("WiFi mode:", 0, 0);
                    lcd.print(WiFi.getMode() == WIFI_STA ? "STA" : "AP", 0, 1);
                }
                break;

                case 10:
                {
                    int clientCount = ProgramWiFi.getConnectedClientCount();

                    lcd.print("Status:", 0, 0);

                    if (WiFi.getMode() == WIFI_STA) {
                        const char* statusWiFiSta = WiFi.status() == WL_CONNECTED
                                                    ? "Connected"
                                                    : "Disconnected";
                        lcd.print(statusWiFiSta, 0, 1);
                    } 
                    else if (WiFi.getMode() == WIFI_AP) {
                        String apStatus = "Client: " + String(clientCount);
                        lcd.print(apStatus.c_str(), 0, 1);
                    } 
                    else {
                        lcd.print("Unknown", 0, 1);
                    }
                }
                break;

                case 11:
                {
                    lcd.print("Date:", 0, 0);
                    lcd.print(rtcDate);
                    lcd.print("Time:", 0, 1);
                    lcd.print(rtcTime);
                }
                break;
            }
            xSemaphoreGive(i2cMutex);
        }
    }
}