/**
 *  @file LEDBoard.h
 *  @version 1.0.0
 *  @date 2024
 *  @author basyair7
 * 
 *  @brief A class for managing the state and behavior of an LED indicator.
 * 
 *  This class provides a singleton interface for controlling an LED, with functionality 
 *  to initialize the LED pin and toggle its state based on a specified delay.
 */

#pragma once

#include <Arduino.h>
#include "variable.h"

#define LEDON   false  ///< Logical state for turning the LED on
#define LEDOFF  true   ///< Logical state for turning the LED off

/**
 * @class LEDBoard
 * @brief Singleton class to manage an LED indicator.
 */
class LEDBoard {
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
         * 
         * This sets up the LED pin as an output and turns the LED off.
         */
        static void begin() {
            instance().__begin__();
        }

        /**
         * @brief Control the LED blinking state (static method).
         * @param state Boolean flag to enable or disable LED blinking.
         * @param __delay__ Delay interval for blinking, in milliseconds.
         */
        static void run(const bool state, const uint32_t __delay__) {
            instance().__run__(state, __delay__);
        }

        /**
         * @brief Led on function
         */
        static void on() {
            digitalWrite(LEDINDIKATOR, LEDON);
        }

        /**
         * @brief Led off function
         */
        static void off() {
            digitalWrite(LEDINDIKATOR, LEDOFF);
        }

    private:
        /**
         * @brief Internal method to initialize the LED pin.
         */
        void __begin__() {
            pinMode(LEDINDIKATOR, OUTPUT);  // Set the LED pin as an output
            digitalWrite(LEDINDIKATOR, LEDOFF);  // Ensure the LED starts in the off state
        }

        /**
         * @brief Internal method to handle LED control logic.
         * @param state Boolean flag indicating whether the LED should blink.
         * @param __delay__ Blinking delay in milliseconds.
         */
        void __run__(const bool state, const uint32_t __delay__) {
            if (state) __runLED__(__delay__);  // Execute LED blinking logic only if state is true
        }

    private:
        bool LEDSTATE = LEDOFF;  ///< Current state of the LED (on or off)

        /**
         * @brief Internal method to toggle the LED state based on a delay.
         * 
         * This function uses the Arduino `millis()` function to manage non-blocking 
         * LED blinking. It toggles the LED state when the delay interval is reached.
         * 
         * @param __delay__ Delay interval in milliseconds for toggling the LED state.
         */
        void __runLED__(const uint32_t __delay__) {
            unsigned long currentMillis = millis();

            // Check if the LED needs to toggle state
            if ((currentMillis - this->__Last_Millis__) >= __delay__) {
                this->__Last_Millis__ = currentMillis; // Update the last toggle time
                this->LEDSTATE = !this->LEDSTATE;     // Toggle the LED state
            }

            // Apply the new LED state
            digitalWrite(LEDINDIKATOR, this->LEDSTATE);
        }

    private:
        unsigned long __Last_Millis__ = 0; ///< Tracks the last time the LED toggled
};
