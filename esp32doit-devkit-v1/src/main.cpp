/**
 *  @file main.cpp
 *  @version 1.0.1
 *  @brief Arduinoフレームワークのメインプログラム。
 *  @details このファイルにはESP32プログラムのメインエントリが含まれます。
 *           このファイルを変更する際は注意してください、プログラム全体の動作に影響を与える可能性があります。
 *  @author basyair7
 *  @date 2026
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

#include "MicroBox_main.h"

/**
 * @brief Mainクラスのsetupに委譲してプログラムを初期化します。
 * @details この関数は`MicroBox_main.h`からの`setup`実装として機能します。
 *          ユーザー定義の初期化ロジックのために`Main::SETUP`関数を呼び出します。
 * @param baudRate シリアル通信のボーレート。
 * @param erase_eeprom EEPROMデータを消去するブールフラグ（デフォルト: false）。
 */
void setup() {
    // シリアル通信速度を115200 bpsに設定してメインプログラムを初期化します。
    // EEPROMプログラムの読み込みは無効（false）です。
    Main::SETUP(115200, false);
}

/**
 * @brief Mainクラスのloopに委譲してメインプログラムループを実行します。
 * @details この関数は`MicroBox_main.h`からの`loop`実装として機能します。
 */
void loop() {
    Main::LOOP();
}