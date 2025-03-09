/**
 *  @file WSHandlers.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/WebServer.h"

/**
 * @brief Handles WebSocket events triggered by the server or client.
 * 
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
void WebServer::onEvent(
    AsyncWebSocket *server, AsyncWebSocketClient *client,
    AwsEventType type, void *arg, uint8_t *data, size_t len
) {
    if (type == WS_EVT_DATA) { 
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        // Validate the data length and ensure it's a text frame
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            if (len > MAX_MESSAGE_SIZE) {
                Serial.println(F("WebSocket message too large, ignoring..."));
                return;
            }

            // Use a stack-allocated buffer for efficiency
            char message[MAX_MESSAGE_SIZE + 1] = {0};
            memcpy(message, data, len);
            message[len] = '\0';

            // Deserialize JSON from the received message
            StaticJsonDocument<300> doc;
            DeserializationError err = deserializeJson(doc, message);
            if (err) {
                Serial.printf("JSON parse error: %s\n", err.c_str());
                return;
            }

            // Handle WebSocket events based on the "event" key in the JSON
            const char *event = doc["event"];
            if (strcmp(event, "data_server") == 0) {
                this->handleDataServerWS(client); // Handle "data_server" event
            } else if (strcmp(event, "toggle_check") == 0) {
                // Add logic for the "toggle_check" event here
            } else if (strcmp(event, "relay_handler") == 0) {
                // Add logic for the "relay_handler" event here
            } else if (strcmp(event, "datetime") == 0) {
                // Add logic for the "datetime" event here
                this->handleRTCServer(client);
            }
        }
    } else if (type == WS_EVT_ERROR) {
        // Log WebSocket error with client ID
        Serial.printf("WebSocket error: %u\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        // Log WebSocket client disconnection
        Serial.printf("WebSocket disconnected: %u\n", client->id());
    }
}

/**
 * @brief Handles raw WebSocket messages.
 * 
 * This function processes WebSocket text frames, logs the received message, 
 * and sends an echo response to all connected clients.
 * 
 * @param arg Additional frame information, including frame type and length.
 * @param data The raw data received in the WebSocket frame.
 * @param len The length of the received data.
 */
void WebServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    // Validate the data length and ensure it's a complete text frame
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        if (len > MAX_MESSAGE_SIZE) {
            Serial.println(F("WebSocket message too large, ignoring..."));
            return;
        }

        // Use a stack-allocated buffer for the message
        char message[MAX_MESSAGE_SIZE + 1] = {0};
        memcpy(message, data, len);
        message[len] = '\0';

        // Log the received WebSocket message
        Serial.printf("Received WebSocket message: %s\n", message);

        // Send an echo response to all connected clients
        String response = String("Echo: ") + message;
        this->ws.textAll(response);
    }
}
