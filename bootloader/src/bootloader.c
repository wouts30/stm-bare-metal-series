#include "common-defines.h"
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/cm3/vector.h>

#define BOOTLOADER_SIZE ( 0x8000U ) //32KiB
#define MAIN_APP_START_ADDRESS ( FLASH_BASE + BOOTLOADER_SIZE )

// //test linking to fail if bootloader is larger than 32 KiB
// const uint8_t data[0x8000] = {0};

static void jump_to_main(void) {
  //first entry not the reset vector but the stack pointer
  //each entry in the vector table is 4 bytes

  // //address of reset
  // uint32_t * reset_vector_entry = (uint32_t *)(MAIN_APP_START_ADDRESS + sizeof(uint32_t));
  // //actual reset counter
  // uint32_t * reset_vector = (uint32_t *)(* reset_vector_entry);

  // // set program coutner to this address with some magic
  // // make a pointer function declaration 
  // typedef void (*reset_vector_fn)(void);
  // //reset_vector is actually a function
  // reset_vector_fn reset_handler = (reset_vector_fn)reset_vector;
  // reset_handler();

  // //making it better asembly still the same
  // uint32_t * main_vector_table = (uint32_t *)(MAIN_APP_START_ADDRESS);
  // typedef void (*app_reset_vector_fn)(void);
  // app_reset_vector_fn app_reset_handler = (app_reset_vector_fn)main_vector_table[1];
  // app_reset_handler();

  //even more expressive using cm3 library
  //have to be carefull of casting to struct pointer due to padding added by compiler 
  //however since all of the items in the struct is uint32 we can safely cast
  vector_table_t * main_vector_table = (vector_table_t *)(MAIN_APP_START_ADDRESS);
  main_vector_table->reset();

  //note: all of them compile to the same assembly
}

int main(void) {
  ////test linking to fail if bootloader is larger than 32 KiB
  // volatile uint8_t x = 0;
  // for (int i = 0; i < 0x8000; i++) {
  //   x += data[i];
  // }
  
  jump_to_main();

  // Never return
  return 0;
}
