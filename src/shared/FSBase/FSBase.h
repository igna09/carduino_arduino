#pragma once

#include <Arduino.h>
#include <FS.h>
#if defined(EXTERNAL_SD)
#include <SD.h>
#else
#include <LittleFS.h>
#endif

#include "shared/Logger/Logger.h"

class FSBase {
    private:

    public:
        FS* _fs;
        bool _fsInitialized;
        Logger* _logger;

        FSBase(Logger*);

        // File getFile(String path);
        bool remove(String path);
        // File createFile(String path);
        File getOrCreateFile(String path, const char* mode = FILE_READ);
        File getOrCreateDirectory(String path);
        bool exists(String path);
        void appendToFile(String path, String message);
};
