/**
 *  @file variable.h
 *  @version 1.0.0
 *  @author basyair7
 *  @date 2024
*/

#pragma once

#include <Arduino.h>

// PIN I/O Components
#define BOOTBUTTON      D3
#define LEDINDIKATOR    D4
#define DHTSENSOR       D5
#define RAINSENSOR      A0

/*
#define FREEPIN1        D0
#define FREEPIN2        D1
#define FREEPIN3        D2
#define FREEPIN4        D6
#define FREEPIN5        D9
#define FREEPIN6        D10
*/

#define INTERVALDAYSDEFAULT 2

// PIN Output for Relay Module
inline const uint16_t RELAY_PINS[2] = {
    D7, D8
};

// Label Name for Relay Module
inline const String VALUE_DEFAULT[2] = {
    "Relay 1", "Relay 2"
};

// PIN I/O Alias for NodeMCU
inline const String PIN_IO_ALIAS[17] = {
    "D3", "D10", "D4", "D9", "D2", "D1",
    "", "", "", "", "", "", "D6", "D7",
    "D5", "D8", "D0"
};

// don't replace it.. Address for EEPROM
#define ADDR_EEPROM_BLYNK_MODE          0x04
#define ADDR_EEPROM_SAVE_STATE_WATERING 0X08
