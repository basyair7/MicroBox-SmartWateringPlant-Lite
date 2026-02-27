/**
 *  @file LEDBoard.h
 *  @version 1.0.1
 *  @brief LEDインジケーターの状態と動作を管理するクラスです。
 *  @details このクラスは、LEDの点灯と消灯を制御するためのシングルトンインターフェースを提供します。
 *  LEDのピンの初期化と、指定された遅延に基づいてLEDの状態を切り替える機能を備えています。
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

#pragma once

#include <Arduino.h>
#include "variable"

#define LEDON  0x1
#define LEDOFF 0x0

/**
 *  @class LEDBoard
 *  @brief LEDインジケーターの状態と動作を管理するクラスです。
 *  @details このクラスは、LEDの点灯と消灯を制御するためのシングルトンインターフェースを提供します。
 *  LEDのピンの初期化と、指定された遅延に基づいてLEDの状態を切り替える機能を備えています。
 */
class LEDBoard {
    uint8_t LED_PIN_OUT;
    unsigned long __Last_Millis = 0; //!< 最後にLEDの状態を切り替えた時間を追跡するための変数

    public:
        /**
         * @brief LEDBoardのシングルトンインスタンス取得用関数。
         * @details この関数は、LEDBoardクラスの唯一のインスタンスを返します。シングルトンパターンを使用して、複数のインスタンスが作成されないようにします。
         * @return LEDBoardクラスのインスタンスへの参照「さんしょう」。
         */
        static LEDBoard &instance() {
            static LEDBoard instance;
            return instance;
        }

        /**
         * @brief LEDボードを初期化します。
         * @details この関数は、LEDのピンを出力モードに設定し、LEDを消灯状態にします。初期化後、LEDは指定されたピンで制御できるようになります。
         * @param pin LEDが接続されているGPIOピン番号。デフォルトはLED_BUILTINです。
         */
        static void BEGIN(uint8_t pin = LED_BUILTIN) {
            instance().begin(pin);   
        }

        /**
         * @brief LEDの点滅状態を制御します。
         * @details この関数は、LEDの点滅状態を制御します。stateがtrueの場合、LEDは_delayで指定された遅延に基づいて点滅します。stateがfalseの場合、LEDは点滅しません。
         * @param state LEDの点滅状態を指定するブール値。trueで点滅、falseで点滅なしになります。
         * @param _delay LEDの点滅間隔をミリ秒単位で指定します。例えば、_delayが500の場合、LEDは500msごとに点滅します。
         */
        static void RUN(const bool state, const uint32_t _delay) {
            if (state) instance().runLED(_delay);
        }

        /**
         * @brief LEDを点灯「てんとう」する関数。
         */
        static void ON() {
            instance().on();
        }

        /**
         * @brief LEDを消灯「しょうとう」する関数。
         */
        static void OFF() {
            instance().off();
        }

    public:
        /**
         * @brief LEDBoardのコンストラクタです。LEDのピンを初期化します。
         * @details このコンストラクタは、LEDのピンを初期化します。初期化後、LEDは指定されたピンで制御できるようになります。
         * @param pin LEDが接続されているGPIOピン番号。デフォルトはLED_BUILTINです。
         */
        void begin(uint8_t pin = LED_BUILTIN) {
            pinMode(pin, OUTPUT); // LEDピンを出力「しゅつりょく」として設定する。
            digitalWrite(pin, LEDOFF); // LEDを消灯「しょうとう」状態「じょうたい」にする。
            this->LED_PIN_OUT = pin;
        }
        
        /**
         * @brief LEDの点滅状態を制御します。
         * @details この関数は、LEDの点滅状態を制御します。stateがtrueの場合、LEDは_delayで指定された遅延に基づいて点滅します。stateがfalseの場合、LEDは点滅しません。
         * @param state LEDの点滅状態を指定するブール値。trueで点滅、falseで点滅なしになります。
         * @param _delay LEDの点滅間隔をミリ秒単位で指定します。例えば、_delayが500の場合、LEDは500msごとに点滅します。
         */
        void run(const bool state, const uint32_t _delay) {
            if (state) this->runLED(_delay);
        }

        /**
         *  @brief LEDを点灯する関数。
         */

        void on() {
            digitalWrite(this->LED_PIN_OUT, LEDON);
        }

        /**
         * @brief LEDを消灯する関数。
         */
        void off() {
            digitalWrite(this->LED_PIN_OUT, LEDOFF);
        }

    private:
        bool LEDSTATE = LEDOFF; //!< LEDの現在の状態を追跡するための変数。LEDOFFは消灯、LEDONは点灯を表します。

        /**
         * @brief LEDの点滅状態を制御するための内部関数です。
         * 
         * @details この関数は、LEDの点滅状態を制御します。LEDの状態は、指定された遅延に基づいて切り替えられます。
         *          LEDが点滅するためには、この関数が定期的に呼び出される必要があります。例えば、ループ内でこの関数を呼び出すことで、LEDの点滅を実現できます。
         * 
         * @param _delay LEDの点滅間隔をミリ秒単位で指定します。例えば、_delayが500の場合、LEDは500msごとに点滅します。
         */
        void runLED(const uint32_t _delay) {
            unsigned long currentMillis = millis();

            // 指定された遅延が経過したかどうかを確認します。
            if ((currentMillis - this->__Last_Millis) >= _delay) {
                this->__Last_Millis = currentMillis; // 最後にLEDの状態を切り替えた時間を更新します。
                this->LEDSTATE = !this->LEDSTATE;    // LEDの状態を切り替えます。点灯から消灯、または消灯から点灯に切り替わります。
            }

            // LEDの状態に基づいて、LEDを点灯または消灯します。
            digitalWrite(this->LED_PIN_OUT, this->LEDSTATE);
        }
};