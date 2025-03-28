#include <libopencm3/stm32/flash.h>
#include "bl_flash.h"

#define APP_SECTOR_START  (2)
#define APP_SECTOR_END    (7)

void bl_flash_erase_app_sectors(void){
    flash_unlock();
    for (uint32_t sector = APP_SECTOR_START; sector < APP_SECTOR_END; sector++) {
        flash_erase_sector(sector, FLASH_CR_PROGRAM_X32); // this does exactly what the reference manual says
    }
    flash_lock();
}

void bl_flash_write(const uint32_t address, const uint8_t * data, const uint32_t length){
    flash_unlock();
    flash_program(address, data, length);
    flash_lock();
}
