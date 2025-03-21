/**
 *  @file tools.cpp
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

#include "MicroBox/software/LFSMemory"

#define enable  true
#define disable false

bool _readConfigState  = disable;
bool _writeConfigState = disable;

/**
 *  readconfig
 *  @param path
 *  @return String
 */
String LFSMemory::readconfig(const String path) {
    while (_writeConfigState) {
        Serial.print(F("readconfig is busy, retrying...\n"));
        delay(10);
    }

    _readConfigState = enable;
    String _val = "";
    File _file = openfile(path, LFS_READ);
    if (_file) {
        // read file content
        size_t _fileSize = _file.size();
        std::unique_ptr<char []> fileBuffer(new char[_fileSize + 1]);
        _file.readBytes(fileBuffer.get(), _fileSize);
        fileBuffer[_fileSize] = '\0';
        _file.close();
        _val = fileBuffer.get();
        delayMicroseconds(50);
    }
    else {
        Serial.printf("Failed to open file %s for reading\n", path.c_str());
        _val = "null";
    }
    
    _readConfigState = disable;
    return _val;
}

/**
 * writeconfig
 * @param path
 * @param valJson
 */
// void LFSMemory::writeconfig(const String path, String valJson) {
//     while (_readConfigState) {
//         Serial.print(F("writeconfig is busy, retrying...\n"));
//         delay(10);
//     }

//     _writeConfigState = enable;
//     this->removefileconfig(path);

//     File _file = openfile(path, LFS_WRITE);
//     if (_file) {
//         _file.write((const uint8_t *)valJson.c_str(), valJson.length());
//         delayMicroseconds(50);
//         _file.close();
//     }
//     else {
//         Serial.printf("Failed to create a new file %s\n", path.c_str());
//     }

//     _writeConfigState = disable;
// }

void LFSMemory::writeconfig(const String path, String valJson) {
    while (_readConfigState) {
        Serial.print(F("writeconfig is busy, retrying...\n"));
        delay(10);
    }

    _writeConfigState = enable;

    // Serial.printf("Writing to %s: %s\n", path.c_str(), valJson.c_str());
    if (LFS.exists(path)) {
        // Serial.printf("Removing old file: %s\n", path.c_str());
        LFS.remove(path);
    }

    File _file = openfile(path, LFS_WRITE);
    if (!_file) {
        Serial.printf("Failed to open file %s for writing!\n", path.c_str());
        _writeConfigState = disable;
        return;
    }

    _file.write((const uint8_t *)valJson.c_str(), valJson.length());
    _file.flush();
    _file.close();
    
    // Serial.println("File written successfully.");
    _writeConfigState = disable;
}


bool LFSMemory::removefileconfig(const String path) {
    bool x = true;
    if (lfsIsExists(path)) {
        x = removeitem(path);
        delayMicroseconds(50);
    }

    return x;
}