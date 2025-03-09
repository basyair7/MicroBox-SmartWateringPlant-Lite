/** 
 *  @file DHTProgram.hh
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
 * 
 *  @copyright
 *  Copyright (C) 2024, basyair7
 * 
 *   This program is free software: you can redistribute it and/or modify
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

#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>

class DHTProgram {
    public:
        float temperature = 0.0, humidity = 0.0;

    public:
        DHTProgram(uint8_t pin, uint8_t type) : __dht_obj__(pin, type) { /*TODO(Not yet implemented)*/ }
        
        /*! initialize device.
        * @param Delay delay program sensor (default 1000ms)
        */
        void begin(uint32_t Delay = 1000UL) {
            if (Delay < 1000UL) {
                Serial.println(F("Delay value cannot be less that 1000ms"));
                return;
            }

            this->__dht_obj__.begin();
            this->__dht_obj__.temperature().getSensor(&this->__sensor__);
            this->__dht_obj__.humidity().getSensor(&this->__sensor__);

            this->__delay__ = Delay >= 1001L ? Delay : this->__sensor__.min_delay / Delay;
            this->information();
            this->__check_begin__ = true;
        }

        // get data sensor event and print it's value
        void running() {
            if (!this->__check_begin__) {
                Serial.println(F("Error: DHTProgram not initialized! Call begin() first"));
                return;
            }

            if ((unsigned long)(millis() - this->__LAST_TIME__) >= (long unsigned int)this->__delay__)
            {
                this->__LAST_TIME__ = millis();
                // getEvent temperature
                this->__dht_obj__.temperature().getEvent(&this->__event__);
                this->temperature = (isnan(this->__event__.temperature)) ? -1 : this->__event__.temperature;
                
                // getEvent Humidity
                this->__dht_obj__.humidity().getEvent(&this->__event__);
                this->humidity = (isnan(this->__event__.relative_humidity)) ? -1 : this->__event__.relative_humidity;
            }
        }

    private:
        // get information device
        void information() {
            Serial.println(F("DHTxx Unified Sensor Example"));
            // print temperature sensor details
            this->__dht_obj__.temperature().getSensor(&this->__sensor__);
            Serial.println(F("------------------------------------"));
            Serial.println(F("Temperature Sensor"));
            Serial.print  (F("Sensor Type\t: ")); Serial.println(this->__sensor__.name);
            Serial.print  (F("Driver Ver\t: "));  Serial.println(this->__sensor__.version);
            Serial.print  (F("Unique ID\t: "));   Serial.println(this->__sensor__.sensor_id);
            Serial.print  (F("Max Value\t: "));   Serial.print(this->__sensor__.max_value);  Serial.println(F(" °C"));
            Serial.print  (F("Min Value\t: "));   Serial.print(this->__sensor__.min_value);  Serial.println(F(" °C"));
            Serial.print  (F("Resolution\t: "));  Serial.print(this->__sensor__.resolution); Serial.println(F(" °C"));
            Serial.println(F("------------------------------------"));
            
            // print humidity sensor details
            this->__dht_obj__.humidity().getSensor(&this->__sensor__);
            Serial.println(F("Humidity Sensor"));
            Serial.print  (F("Sensor Type\t: ")); Serial.println(this->__sensor__.name);
            Serial.print  (F("Driver Ver\t: "));  Serial.println(this->__sensor__.version);
            Serial.print  (F("Unique ID\t: "));   Serial.println(this->__sensor__.sensor_id);
            Serial.print  (F("Max Value\t: "));   Serial.print(this->__sensor__.max_value);  Serial.println(F(" %"));
            Serial.print  (F("Min Value\t: "));   Serial.print(this->__sensor__.min_value);  Serial.println(F(" %"));
            Serial.print  (F("Resolution\t: "));  Serial.print(this->__sensor__.resolution); Serial.println(F(" %"));
            Serial.println(F("------------------------------------"));
        }

    private:
        DHT_Unified __dht_obj__;
        sensor_t __sensor__;
        sensors_event_t __event__;
        uint32_t __delay__;
        unsigned long __LAST_TIME__;
        bool __check_begin__;
};
