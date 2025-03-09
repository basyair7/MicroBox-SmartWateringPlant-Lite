/**
 *  @file WSHandlers.cpp
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

#include "MicroBox/software/WebServer"

/**
 * @brief Handlers WebSocket events triggered by the server or client.
 * @details
 * This function processes WebSocket events such as receiving data, encountering errors,
 * or handling client disconnections. For data events, it validates the payload, parses
 * the JSON structure, and triggers the appropriate event handler.
 * 
 * @param server Pointer to the WebSocket server instance.
 * @param client Pointer to the client initiating the event.
 * @param type The type of WebSocket event (e.g., data, error, disconnect).
 * @param arg Additional information related to the event.
 * @param data The raw data received in the WebSocket frame.
 * @param len The length of the received data.
 */
void WebServerClass::onEvent(
    AsyncWebSocket *server, AsyncWebSocketClient *client,
    AwsEventType type, void *arg, uint8_t *data, size_t len
) {
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        // Validate the data length and ensure it's a text frame
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            if (len > MAX_MESSAGE_SIZE) {
                Serial.println(F("WebSocket message too large, ignoring..."));
                return;
            }

            // Use a stack-allocated buffer for efficiency
            char message[MAX_MESSAGE_SIZE + 1] = { 0 };
            memcpy(message, data, len);
            message[len] = '\0';

            // Deserialize JSON from the received message
            StaticJsonDocument<300> doc;
            DeserializationError error = deserializeJson(doc, message);
            if (error) {
                Serial.printf("JSON parse error: %s", error.c_str());
                return;
            }

            // Handle WebSocket events based on the "event" key in the JSON
            const char *event = doc["event"];
            if (strcmp(event, "data_server") == 0) {
                this->handleDataServeWS(client); // Handle "data_server" event
            }
            else if (strcmp(event, "toggle_check") == 0) {
                // Add logic for the "toggle_check" event here
            }
            else if (strcmp(event, "relay_handler") == 0) {
                // Add logic for the "relay_handler" event here
            }
        }
    }
    else if (type == WS_EVT_ERROR) {
        // Log WebSocket error with client ID
        Serial.print(F("WebSocket error: "));
        Serial.println(client->id());
    }
    else if (type == WS_EVT_DISCONNECT) {
        // Log WebSocket client disconnection
        Serial.print(F("WebSocket disconnected: "));
        Serial.println(client->id());
    }
}

/**
 * @brief Handlers raw WebSocket messages.
 * @details
 * This function processes WebSocket text frames, logs the received message,
 * and sends an echo response to all connected clients.
 * 
 * @param arg Additional frame information, including frame type and length.
 * @param data The raw data received in the WebSocket frame.
 * @param len The length of the received data.
 */
void WebServerClass::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // Validate the data length and ensure it's a complete text frame
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (len > MAX_MESSAGE_SIZE) {
            Serial.println(F("WebSocket message too large, ignoring..."));
            return;
        }

        // Use a stack-allocated buffer for the message
        char message[MAX_MESSAGE_SIZE + 1] = { 0 };
        memcpy(message, data, len);
        message[len] = '\0';

        // Log the received WebSocket message
        Serial.printf("Received WebSocket message: %s\n", message);

        // Send an echo response to all connected clients
        String response = String("Echo: ") + message;
        this->ws.textAll(response);
    }
}