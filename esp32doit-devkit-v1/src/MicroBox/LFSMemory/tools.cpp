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
    if (_writeConfigState) {
        Serial.print(F("readconfig is busy, "));
        Serial.println(F("please check writeconfig method..."));
        return "null";
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
void LFSMemory::writeconfig(const String path, String valJson) {
    if (_readConfigState) {
        Serial.print(F("writeconfig is busy, "));
        Serial.println(F("please check readconfig method..."));
        return;
    }

    _writeConfigState = enable;
    this->removefileconfig(path);

    File _file = openfile(path, LFS_READ);
    if (_file) {
        _file.write((const uint8_t *)valJson.c_str(), valJson.length());
        delayMicroseconds(50);
        _file.close();
    }
    else {
        Serial.printf("Failed to create a new file %s\n", path.c_str());
    }

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