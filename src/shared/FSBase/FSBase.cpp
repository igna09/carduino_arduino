#include "FSBase.h"

FSBase::FSBase() {
    this->_fs = nullptr;
    this->_fsInitialized = false;
};

void FSBase::setupFSBase(Logger* logger) {
    _logger = logger;
    #if defined(EXTERNAL_SD)
    _fs = &SD;
    if(!SD.begin(25)) {
        logger->printlnWrapper("SD Mount Failed");
        return;
    } else{
        logger->printlnWrapper("SD Mount Done");
    }
    #else
        fs = &LittleFS;
        #ifdef ESP8266
        if(!LittleFS.begin()){
            logger->printlnWrapper("LittleFS Mount Failed");
            return;
        } else{
            logger->printlnWrapper("LittleFS Mount Done");
        }    
        #elif defined(ESP32)
        if(!LittleFS.begin(true)){
            logger->printlnWrapper("LittleFS Mount Failed");
            return;
        } else{
            logger->printlnWrapper("LittleFS Mount Done");
        }    
        #endif
    #endif

    this->_fsInitialized = true;
};

bool FSBase::exists(String path) {
    return _fs->exists(path);
}

bool FSBase::remove(String path) {
    if(!exists(path)) {
        return false;
    }
    bool isDir = false;
    File tmp = _fs->open(path);
    isDir = tmp.isDirectory();
    tmp.close();
    if(isDir) {
        return _fs->rmdir(path);
    } else {
        return _fs->remove(path);
    }
}

File FSBase::getOrCreateFile(String path, const char* mode) {
    return _fs->open(path, mode, true);
}

File FSBase::getOrCreateDirectory(String path) {
    if(!exists(path)) {
        _fs->mkdir(path);
    }
    return _fs->open(path);
}

void FSBase::appendToFile(String path, String message) {
    File file = getOrCreateFile(path, FILE_APPEND);
    file.println(message);
    file.close();
}
