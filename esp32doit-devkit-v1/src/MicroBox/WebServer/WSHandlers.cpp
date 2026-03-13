/**
 *  @file WSHandlers.cpp
 *  @version 1.0.1
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

#include "MicroBox/software/WebServer"

/**
 * @brief サーバーまたはクライアントによって発生した WebSocket イベントを処理する。
 * @details
 * 本館数は、データ受信、エラー発生、クライアント切断などの WebSocket イベントを処理する。
 * データイベントにおいては、ペイロードを検証し、JSON 構造を解析した上で、適切なイベントハンドラを呼び出す。
 * 
 * @param server 本イベントを発生させたWebSocketサーバーインスタンスを指すポインタ。
 * @param client イベントに関連するクライアントを指すポインタ。クライアントIDや接続状態などの情報を含む。
 * @param type 発生したイベントの種類を示す列挙型。例: データ受信、エラー、切断など。
 * @param arg 本イベントに関する付加情報。
 * @param data イベントに関連する生データを指すポインタ。通常は、受信したメッセージのペイロードを指す。
 * @param len データが指すデータの長さをバイト単位で示す。
 */
void WebServerClass::onEvent(
    AsyncWebSocket *server, AsyncWebSocketClient *client,
    AwsEventType type, void *arg, uint8_t *data, size_t len
) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        // 受信したWebSocketメッセージが完全なテキストフレームであることを確認する。
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            if (len > MAX_MESSAGE_SIZE) {
                Serial.println(F("WebSocket message too large, ignoring..."));
                return;
            }

            // 受信したメッセージをスタック上のバッファにコピーする。これにより、動的メモリ割り当てを回避し、パフォーマンスを向上させる。
            char message[MAX_MESSAGE_SIZE + 1] = { 0 };
            memcpy(message, data, len);
            message[len] = '\0';

            // 受信したWebSocketメッセージをログに出力する。
            StaticJsonDocument<300> doc;
            DeserializationError error = deserializeJson(doc, message);
            if (error) {
                Serial.printf("JSON parse error: %s", error.c_str());
                return;
            }

            // 受信したJSONメッセージからイベントタイプを抽出し、対応するイベントハンドラを呼び出す。
            const char *event = doc["event"];
            if (strcmp(event, "data_server") == 0) {
                this->handleDataServeWS(client); // Handle "data_server" event
            }
            else if (strcmp(event, "toggle_check") == 0) {
                // Add logic for the "toggle_check" event here
                // 例: トグルスイッチの状態を更新し、必要に応じてリレーの状態を変更するなど
            }
            else if (strcmp(event, "relay_handler") == 0) {
                // Add logic for the "relay_handler" event here
                // 例: リレーの状態を更新し、必要に応じて関連するデータをクライアントに送信するなど
            }
            else if (strcmp(event, "datetime") == 0) {
                this->handleRTCServer(client); // Handle "datetime" event
            }
        }
    }
    else if (type == WS_EVT_ERROR) {
        // クライアントIDを付加してWebSocketエラーを記録する。
        Serial.print(F("WebSocket error: "));
        Serial.println(client->id());
    }
    else if (type == WS_EVT_DISCONNECT) {
        // クライアントIDを付加してWebSocket切断イベントを記録する。
        Serial.print(F("WebSocket disconnected: "));
        Serial.println(client->id());
    }
}

/**
 * @brief WebSocketのテキストフレームを処理する。受信したメッセージをログに出力し、すべての接続されたクライアントにエコーメッセージを送信する。
 * @details
 * この関数は、WebSocketサーバーがテキストフレームを受信したときに呼び出される。受信したメッセージをスタック上のバッファにコピーし、ログに出力する。
 * その後、すべての接続されたクライアントに対して、受信したメッセージをエコーするレスポンスを送信する。
 * 
 * @param arg イベントに関連する付加情報を指すポインタ。通常は、WebSocketフレームの情報を含む構造体を指す。
 * @param data WebSocketフレームで受信した生データ。
 * @param len データが指すデータの長さをバイト単位で示す。
 */
void WebServerClass::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // 受信したWebSocketメッセージが完全なテキストフレームであることを確認する。
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (len > MAX_MESSAGE_SIZE) {
            Serial.println(F("WebSocket message too large, ignoring..."));
            return;
        }

        // 受信したメッセージをスタック上のバッファにコピーする。これにより、動的メモリ割り当てを回避し、パフォーマンスを向上させる。
        char message[MAX_MESSAGE_SIZE + 1] = { 0 };
        memcpy(message, data, len);
        message[len] = '\0';

        // 受信したWebSocketメッセージをログに出力する。
        Serial.printf("Received WebSocket message: %s\n", message);

        // すべての接続されたクライアントに対して、受信したメッセージをエコーするレスポンスを送信する。
        String response = String("Echo: ") + message;
        this->ws.textAll(response);
    }
}
