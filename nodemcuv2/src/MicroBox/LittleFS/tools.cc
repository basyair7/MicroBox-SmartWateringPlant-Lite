/**
 *  @file tools.cc
 *  @version 1.0.1
 *  @author basyair7
 *  @date 2024
*/

#include "MicroBox/LFSProgram.h"

#define ENABLE  true
#define DISABLE false

bool __readconfigstate  = DISABLE;
bool __writeconfigstate = DISABLE;

/*! readconfig
 * @param path
 * @return String
*/
String LFSProgram::readconfig(const String path) {
    if (__writeconfigstate) {
        Serial.println(F("readconfig is bussy, please check writeconfig method..."));
        return "null";
    }

    __readconfigstate = ENABLE;
    String __val__ = "";
    File _file = openfile(path, LFS_READ);
    if (_file) {
        // read file content
        size_t _fileSize = _file.size();
        std::unique_ptr<char []> fileBuffer(new char[_fileSize + 1]);
        _file.readBytes(fileBuffer.get(), _fileSize);
        _file.close();
        __val__ = fileBuffer.get();
    }
    else {
        Serial.printf("Failed to open file %s for reading\n", path.c_str());
        __val__ = "null";
    }
    __readconfigstate = DISABLE;
    return __val__;
}

/*! writeconfig
 * @param path
 * @param valJson
*/
void LFSProgram::writeconfig(const String path, String valJson) {
    if (__readconfigstate) {
        Serial.println(F("writeconfig is busy, please check readconfig method..."));
        return;
    }

    __writeconfigstate = ENABLE;
    
    this->removefileconfig(path);
    
    File _file = openfile(path, LFS_WRITE);
    if (_file) {
        _file.write((const uint8_t *)valJson.c_str(), valJson.length());
        delayMicroseconds(50);
        _file.close();
    }
    else {
        Serial.printf("Failed to create a new file %s\n", path.c_str());
    }

    __writeconfigstate = DISABLE;
}

bool LFSProgram::removefileconfig(const String path) {
    if (lfsIsExists(path)) {
        bool x = removeitem(path);
        delayMicroseconds(50);
        return x;
    }
    else return false;
}
