/**
 *  @file MicroBox_main.h
 *  @version 1.0.1
 *  @brief このファイルは、ESP32プログラムのメインエントリーポイントとして機能するクラスを定義します。
 *  @details このクラスは、Arduinoのsetup()とloop()関数をラップし、EEPROMの消去機能も提供します。
 * 
 *  @author basyair7
 *  @date 2026
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

#pragma once

#include <Arduino.h>
#include "EraseEEPROM"

class MicroBox_Main {
    /****** ADD NEW FUNCTION HERE / ここに新しい関数を追加する。 ******/
    // void __example__(void);
    
    /****** DON'T MODIFY THIS METHOD's FUNCTION (BE CAREFUL)! ******/
    /****** このメソッドの機能を変更しないこと（注意）！******/
    protected:
        /**
         * @brief システムをリブートする。
         * @param baud シリアル通信のボーレート。
         */
        void setup(unsigned long baud);

        /**
         * @brief システムをリブートする。
         */
        void loop();
};

/**
 *  @class Main
 *  @brief このクラスは、Arduinoフレームワークのメインセットアップとループ機能を処理します。
 *  @details このクラスは、EEPROMの消去機能も提供し、必要に応じてEEPROMを消去してからセットアップを実行できます。
 */
class Main : protected MicroBox_Main {
    /****** DON'T MODIFY THIS METHOD's FUNCTION (BE CAREFUL)! ******/
    /****** このメソッドの機能を変更しないこと（注意）！******/
    bool _erase_eeprom_prog = false;
    
    /**
     *  @brief EEPROMを消去するための静的メソッド。
     *  @param baud シリアル通信のボーレート。
     */
    static void EraseEEPROM(unsigned long baud) {
        Serial.begin(baud);

        // EEPROMの初期化を試みる。
        if (!EraseEEPROM::BEGIN()) {
            Serial.println(F("Failed to initialize EEPROM!\nThe run() function cannot be executed."));
            return;
        }

        Serial.println(F("EEPROM successfully initialized."));

        // EEPROMの内容を消去する。
        Serial.println(F("Erasing EEPROM..."));
        EraseEEPROM::RUN();
        Serial.println(F("EEPROM successfully erased."));
    }

    public:
        /**
         *  @brief Mainクラスのシングルトンインスタンスを取得する。
         *  @return Main& シングルトンインスタンスへの参照。
         */
        static Main &instance() {
            static Main instance;
            return instance;
        }

        /**
         *  @brief セットアップ関数。EEPROMの消去フラグに基づいて、EEPROMを消去してからセットアップを実行するかどうかを決定します。
         *  @param baud シリアル通信のボーレート。
         *  @param erase_eeprom EEPROMを消去するかどうかのフラグ（デフォルトはfalse）
         */
        static void SETUP(unsigned long baud, bool erase_eeprom = false)
        {
            if (erase_eeprom) {
                instance()._erase_eeprom_prog = erase_eeprom;
                Main::EraseEEPROM(baud);
            }

            if (!instance()._erase_eeprom_prog)
                instance().setup(baud);
        }

        /**
         * @brief ループ関数。EEPROM消去プログラムが実行されていない場合にのみ、通常のループ処理を実行します。
         */
        static void LOOP() {
            if (!instance()._erase_eeprom_prog)
                instance().loop();
        }
};