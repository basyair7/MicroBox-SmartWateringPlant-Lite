/**
 *  @file LEDBoard.h
 *  @version 1.0.0
 *  @brief A class for managing the state and behavior of an LED indicator.
 *  @details This class provides a singleton interface for controlling and LED, with functionality
 *           to initialize the LED pin and toggle it's state based on a specified delay.
 *  
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

#pragma once

#include <Arduino.h>
#include "variable"

#define LEDON  0x1
#define LEDOFF 0x0

/**
 *  @class LEDBoard
 *  @brief Singleton class to manage an LED indicator.
 */
class LEDBoard {
    uint8_t LED_PIN_OUT;
    unsigned long __Last_Millis = 0; //!< Tracks the last time the LED toggled

    public:
        /**
         * @brief Retrieve the singleton instance of the class.
         * @return LEDBoard& Reference to the singleton instance.
         */
        static LEDBoard &instance() {
            static LEDBoard instance;
            return instance;
        }

        /**
         * @brief Initialize the LED board (static method).
         * @details This sets up the LED pin as an output and turns the LED off.
         * @param pin Pinout LED
         */
        static void BEGIN(uint8_t pin = LED_BUILTIN) {
            instance().begin(pin);   
        }

        /**
         * @brief Control the LED blinking state (static method).
         * @param state Boolean flag to enable or disable LED blinking.
         * @param _delay Delay interval of blinking, in milliseconds.
         */
        static void RUN(const bool state, const uint32_t _delay) {
            if (state) instance().runLED(_delay);
        }

        /**
         * @brief LED on function
         */
        static void ON(uint8_t pin_led = LED_BUILTIN) {
            instance().on(pin_led);
        }

        /**
         * @brief LED off function
         */
        static void OFF(uint8_t pin_led = LED_BUILTIN) {
            instance().off(pin_led);
        }

    public:
        /**
         * @brief Initialize the LED board.
         * @details This sets up the LED pin as an output and turns the LED off.
         * @param pin Pinout LED
         */
        void begin(uint8_t pin = LED_BUILTIN) {
            pinMode(pin, OUTPUT); // Set the LED pin as an output
            digitalWrite(pin, LEDOFF); // Ensure the LED start in the off state
            this->LED_PIN_OUT = pin;
        }
        
        /**
         * @brief Control the LED blinking state.
         * @param state Boolean flag to enable or disable LED blinking.
         * @param _delay Delay interval of blinking, in milliseconds.
         */
        void run(const bool state, const uint32_t _delay) {
            if (state) this->runLED(_delay);
        }

        /**
         *  @brief LED on function
         */

        void on(uint8_t led_pin = LED_BUILTIN) {
            digitalWrite(led_pin, LEDON);
        }

        /**
         * @brief Led off function
         */
        void off(uint8_t led_pin = LED_BUILTIN) {
            digitalWrite(led_pin, LEDOFF);
        }

    private:
        bool LEDSTATE = LEDOFF; //!< Current state of the LED (on or off)

        /**
         * @brief Internal method to toggle the LED state based on a delay.
         * 
         * @details This function uses the Arduino `millis()` function to manage non-blocking
         *          LED blinking. It toggles the LED state when the delay interval is reached.
         * 
         * @param _delay Delay internal in milliseconds for toggling the LED state.
         */
        void runLED(const uint32_t _delay) {
            unsigned long currentMillis = millis();

            // Check if the LED needs to toggle state
            if ((currentMillis - this->__Last_Millis) >= _delay) {
                this->__Last_Millis = currentMillis; // Update the last toggle time
                this->LEDSTATE = !this->LEDSTATE;    // Toggle the LED state
            }

            // Apply the new LED state
            digitalWrite(this->LED_PIN_OUT, this->LEDSTATE);
        }
};