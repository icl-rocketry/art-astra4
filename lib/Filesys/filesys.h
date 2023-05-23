#pragma once
// This is a refactored version of this example: https://github.com/adafruit/Adafruit_TinyUSB_Arduino/tree/master/examples/MassStorage/msc_external_flash

#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <SPI.h>
#include <SdFat.h>

// The code here is a little (very) bad. This is because it'll be replaced soon
// with better code from libriccore

// Namespacing so you don't have random calls to "open()"
namespace filesys {
void init();

enum FileMode {
    READONLY = 1,
    WRITEONLY = 2,
    APPENDONLY = 3
};

File32 open(const String& filename, FileMode mode);
bool exists(const String& filename);
};  // namespace filesys
