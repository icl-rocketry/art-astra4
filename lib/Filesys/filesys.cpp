#include "filesys.h"

namespace filesys {
// All of these unfortunately need to be global variables since they are used by 
// msc_read_cb, msc_write_cb and msc_flush_cb
Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash flash(&flashTransport);
FatVolume fatfs;
Adafruit_USBD_MSC usb_msc;

int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
    // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
    // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
    return flash.readBlocks(lba, (uint8_t*)buffer, bufsize / 512) ? bufsize : -1;
}

int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
    // Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks
    // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
    return flash.writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
}

void msc_flush_cb(void) {
    // sync with flash
    flash.syncBlocks();

    // clear file system's cache to force refresh
    fatfs.cacheClear();
}

void init() {
    flash.begin();

    // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
    usb_msc.setID("Adafruit", "External Flash", "1.0");

    // Set callback
    usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);

    // Set disk size, block size should be 512 regardless of spi flash page size
    usb_msc.setCapacity(flash.size() / 512, 512);

    // MSC is ready for read/write
    usb_msc.setUnitReady(true);

    usb_msc.begin();

    // Init file system on the flash
    bool fs_formatted = fatfs.begin(&flash);
}

File32 open(const String& filename, FileMode mode) {
    oflag_t mode_;
    switch (mode) {
    case FileMode::READONLY:
        mode_ = O_READ;
        break;
    case FileMode::WRITEONLY:
        mode_ = O_WRITE;
        break;
    case FileMode::APPENDONLY:
        mode_ = O_APPEND;
        break;
    }
    return fatfs.open(filename, mode_);
}

bool exists(const String& filename) {
    return fatfs.exists(filename);
}
};