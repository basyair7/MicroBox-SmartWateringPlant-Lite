/** 
 *  @file DS3231rtc.hh
 *  @version 1.0.1
 *  @date 2024
 *  @author basyair7
 * 
 *  @copyright
 *  Copyright (C) 2024, basyair7
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
 *  along with this program. If not, see <https://www.gnu.org/licenses/>
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <RTClib.h>

/**
 * @class DS3231rtc
 * @brief A wrapper class for managing the DS3231 RTC module using RTClib.
 */
class DS3231rtc {
    public:
        bool twelve_hour_format;
        
        /**
         * @brief Initialize the DS3231 RTC module.
         * 
         * This function attempts to initialize the RTC module. If the module is not 
         * found, it continuously retries with a delay. Additionally, if the RTC has 
         * lost power, it resets the time to the compile time.
         * 
         * @param twelve_hour_format
         */
        void begin(bool twelve_hour_format = false) {
            while (!this->__rtc__.begin()) {
                Serial.println(F("Status : Couldn't find RTC"));
                delay(100);
            }

            if (this->__rtc__.lostPower()) {
                Serial.println(F("Status : RTC Lost Power, setting the time!"));
                this->__rtc__.adjust(DateTime(F(__DATE__), F(__TIME__)));
            }

            this->twelve_hour_format = twelve_hour_format;
            Serial.println();
            Serial.println(F("DS3231rtc Initializing..."));
            Serial.printf("Date : %s\nTime : %s\n\n\n", this->datestr().c_str(), this->timestr().c_str());
        }

        /**
         * @brief Get the current date and time as a DateTime object.
         * @return DateTime The current date and time.
         */
        DateTime now() {
            return this->__rtc__.now();
        }

        /**
         * @brief Manually adjust the RTC time.
         * 
         * @param month The month (1-12).
         * @param day The day of the month (1-31).
         * @param year The full year (e.g., 2024).
         * @param hour The hour (0-23).
         * @param minute The minute (0-59).
         * @param second The second (0-59).
         */
        void manualAdjust(
            uint8_t month, uint8_t day, uint16_t year,
            uint8_t hour, uint8_t minute, uint8_t second
        ) {
            this->__rtc__.adjust(DateTime(year, month, day, hour, minute, second));
        }

        /**
         * @brief Automatically adjust the RTC time to the compile date and time.
         */
        void autoAdjust(void) {
            this->__rtc__.adjust(DateTime(F(__DATE__), F(__TIME__)));
        }

        /**
         * @brief Get the current date components (month, day, year).
         * 
         * @tparam T Data type for the parameters (e.g., uint8_t, int).
         * @param month Pointer to store the current month.
         * @param day Pointer to store the current day.
         * @param year Pointer to store the current year.
         */
        template <typename T>
        void date(T *month = nullptr, T *day = nullptr, T *year = nullptr) {
            if (month != nullptr) *month = this->now().month();
            if (day != nullptr)   *day   = this->now().day();
            if (year != nullptr)  *year  = this->now().year(); 
        }

        /**
         * @brief Get the current time components (hour, minute, second).
         * 
         * @tparam T Data type for the parameters (e.g., uint8_t, int).
         * @param hour Pointer to store the current hour.
         * @param minute Pointer to store the current minute.
         * @param second Pointer to store the current second.
         */
        template <typename T>
        void time(T *hour = nullptr, T *minute = nullptr, T *second = nullptr, String *am_pm = nullptr) {
            if (!this->twelve_hour_format) {
                if (hour != nullptr)  *hour  = this->now().hour();
            }
            else {
                if (am_pm != nullptr) *am_pm = (this->now().hour() < 12) ? "AM" : "PM";
                if (hour != nullptr)  *hour  = this->hours();
            }

            if (minute != nullptr) *minute = this->now().minute();
            if (second != nullptr) *second = this->now().second();
        }

        /**
         * @brief Get the current date as a formatted string (MM/DD/YYYY).
         * @return String The formatted date string.
         */
        String datestr(void) {
            return String(this->now().month()) + "/"
                + String(this->now().day()) + "/"
                + String(this->now().year());
        }

        /**
         * @brief Get the current time as a formatted string (HH:MM:SS).
         * @return String The formatted time string.
         */
        String timestr(void) {
            if (!this->twelve_hour_format) {
                return String(this->now().hour()) + ":"
                    + String(this->now().minute()) + ":"
                    + String(this->now().second());
            }

            return String(this->hours()) + ":"
                + String(this->now().minute()) + ":"
                + String(this->now().second()) + " "
                + (this->now().hour() < 12 ? "AM" : "PM");
        }

        /**
         * @brief Get the current day of the week as a string.
         * @return String The name of the current day of the week.
         */
        String getDayOfWeek(void) {
            return this->__listDayOfWeek__[this->now().dayOfTheWeek()];
        }

    private:
        RTC_DS3231 __rtc__; ///< Instance of the RTClib RTC_DS3231 class.
        const char __listDayOfWeek__[7][12] = { // Day names in English
            "Sunday", "Monday", "Tuesday",
            "Wednesday", "Thursday", "Friday",
            "Saturday"
        };
        
        uint8_t hours() { // convert the hours component to 12-hour format
            uint8_t __hours__ = this->now().hour();
            return (__hours__ > 12) ? __hours__ - 12 : (__hours__ == 0 ? 12 : __hours__);
        }
};
