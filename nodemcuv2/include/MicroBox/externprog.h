/**
 *  @file externprog.h
 *  @version 1.0.0
 *  @author basyair7
 *  @date 2024
*/

#pragma once

#include "ProgramWiFi.h"
#include "LFSProgram.h"
#include "RainCheck.h"
#include "DHTProgram.hh"
#include "DS3231rtc.hh"
#include "WateringSys.h"

extern ProgramWiFi wifiprog;
extern LFSProgram lfsprog;
extern RainCheck rainCheck;
extern DHTProgram dhtprog;
extern DS3231rtc rtcprog;
extern WateringSys wateringsys;
extern bool RebootState;
extern unsigned long __lastTimeReboot__;
extern float __freeHeapMemory__, __totalHeapMemory__;
