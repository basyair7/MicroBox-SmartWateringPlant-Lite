/**
 *  @file RebootSys.h
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LEDBoard.h"
#include "MyEEPROM.hh"
#include "externprog.h"

class RebootSys {
    public:
        static RebootSys &instance() {
            static RebootSys instance;
            return instance;
        }

        static void run(unsigned long *LastTime, bool state) {
            instance().__run(LastTime, state);
        }

    private:
        void __run(unsigned long *LastTime, bool state) {
            static unsigned long _previous_Millis = 0;
            static int _count_down = 3;
            static bool _message_Printed = false;

            if (state) {
                if (!_message_Printed) {
                    Serial.println(F("\n\nReboot System... Please wait\n"));
                    _message_Printed = true;
                }

                unsigned long _elapsed = millis() - *LastTime;

                if (_elapsed <= 3000 && millis() - _previous_Millis >= 1000) {
                    _previous_Millis = millis();
                    Serial.println(_count_down--);
                }
                
                else if (_elapsed > 6000 && _elapsed <= 9000) {
                    ESP.wdtDisable();
                    delay(1000);
                    ESP.restart();
                }
            }
        }
};

/**
 * @class AutoChangeState
 * @brief Monitors Wi-Fi status and executes actions if the connection remains lost for a specified timeout duration.
 */
class AutoChangeState {
    public:
        static AutoChangeState& instance() {
            static AutoChangeState instance;
            return instance;
        }

        static void run(uint32_t TIMEOUT = 30) {
            // Ensure the singleton instance runs the logic
            instance().__run(TIMEOUT);
        }

    private:
        void __run(uint32_t TIMEOUT) {
            if (TIMEOUT <= 1) {
                Serial.println(F("Timeout must be greater than 1 minute."));
                return;
            }

            // Convert timeout to milliseconds
            this->timeout = TIMEOUT * 60 * 1000;

            // Update autoChangeEnabled state every 5 seconds
            static unsigned long lastReadTime = 0;
            if ((millis() - lastReadTime) >= 5000) {
                lfsprog.readConfigState(AUTOCHANGE, &this->autoChangeEnabled);
                lastReadTime = millis();
            }

            // Proceed only if autoChange is enabled
            if (this->autoChangeEnabled && WiFi.getMode() == WIFI_STA) {
                this->changeState();
            }
        }

        void changeState() {
            if (WiFi.status() == WL_CONNECTED) {
                // Reset the timer only if Wi-Fi is reconnected
                startMillis = millis();
                // Serial.println(F("Wi-Fi connected. Timer reset."));
                LEDBoard::off();
            } else {
                // If Wi-Fi is disconnected
                LEDBoard::run(true, 1000);
                unsigned long elapsed = millis() - startMillis;

                if (elapsed >= timeout) {
                    // Execute action after timeout
                    Serial.println(F("Wi-Fi disconnected for 30 minutes. Saving state and resetting ESP."));
                    eeprom.save_state_blynk(false);
                    delay(100);  // Ensure data is saved
                    ESP.reset(); // Perform reset
                } else {
                    // Calculate remaining time and convert to minutes and seconds
                    unsigned long remaining = timeout - elapsed;
                    remaining /= 1000; // Convert from milliseconds to seconds

                    unsigned long minutes = remaining / 60; // Calculate minutes
                    unsigned long seconds = remaining % 60; // Calculate seconds

                    this->PrintOutRemaining(minutes, seconds);
                }
            }
        }

        void PrintOutRemaining(unsigned long minutes, unsigned long seconds) {
            static unsigned long lastPrintTime = 0; 
            if (millis() - lastPrintTime >= 1000L) {
                lastPrintTime = millis();

                // Print remaining time in MM:SS format
                Serial.print(F("Wi-Fi disconnected. Remaining time: "));
                if (minutes < 10) {
                    Serial.print(F("0"));
                }
                Serial.print(minutes);
                Serial.print(F(":"));
                if (seconds < 10) {
                    Serial.print(F("0"));
                }
                Serial.println(seconds);
            }
        }

    private:
        MyEEPROM eeprom;                      ///< Instance of MyEEPROM for EEPROM operations
        bool autoChangeEnabled = false;       ///< Flag indicating whether autoChange is enabled
        unsigned long timeout = 0;            ///< Timeout value in milliseconds
        unsigned long startMillis = millis(); ///< Start time of Wi-Fi disconnection
};
