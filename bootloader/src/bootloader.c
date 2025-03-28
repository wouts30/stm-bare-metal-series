#include "common-defines.h"
#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/cm3/vector.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "core/system.h"
#include "core/uart.h"
#include "comms.h"

#define UART_PORT (GPIOA)
#define UART_TX_PIN (GPIO9)
#define UART_RX_PIN (GPIO10)

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

static void GPIO_setup(void){
  rcc_periph_clock_enable(RCC_GPIOA);

  gpio_mode_setup(UART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_TX_PIN | UART_RX_PIN);
  gpio_set_af(UART_PORT, GPIO_AF7, UART_TX_PIN | UART_RX_PIN);

}

int main(void) {
  ////test linking to fail if bootloader is larger than 32 KiB
  // volatile uint8_t x = 0;
  // for (int i = 0; i < 0x8000; i++) {
  //   x += data[i];
  // }
  
  
  system_setup();
  GPIO_setup();
  uart_setup();
  comms_setup();

  comms_packet_t packet = {
    .length = 9,
    .data = {1,2,3,4,5,6,7,8,9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    .crc = 0
  };
  packet.crc = comms_copute_crc(&packet);
  // packet.crc++;
  comms_packet_t rxpacket;

  while (1) {
    comms_update();
    if (comms_packets_available()) {
      comms_read(&rxpacket);
    }
    comms_write(&packet);//check crc file for logpoint //TODO: check ITM
    system_delay(500);
    // comms_update();
  }
  //never unconfigure the gpio and setups that could cause interrupts to trigger 
  // this can cause some problems if not configured to handel things in program
  
  jump_to_main();

  // Never return
  return 0;
}
