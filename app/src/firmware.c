#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/scb.h>//SCB_VTOR ( vector table offset register)

#include "core/system.h"
#include "core/timer.h"

#define LED_PORT (GPIOC)
#define LED_PIN (GPIO13)

#define EXT_LED_PORT (GPIOA)
#define EXT_LED_PIN (GPIO0)

#define BOOTLOADER_SIZE ( 0x8000U )

// This is because when interrupts occur it will look at the vector table of the bootloader as it does not know of its own
// and it is not the first code that is run the bootloader is
// This tells the application that its vector table sits at 0x8000
static void vector_setup(void){
  SCB_VTOR = BOOTLOADER_SIZE;
}


static void GPIO_setup(void){
  vector_setup();
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOA);

  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);

  gpio_mode_setup(EXT_LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EXT_LED_PIN);
  gpio_set_af(EXT_LED_PORT, GPIO_AF1, EXT_LED_PIN);
}

int main(void){
  system_setup();

  GPIO_setup();
  timer_setup();

  uint64_t start = system_get_ticks();
  float duty_cycle = 50.0f;
  timer_set_pwm_duty_cycle(duty_cycle);

  int i = 0;
  while(1){
    if (system_get_ticks() - start >= 10){
      if (i == 100){
        gpio_toggle(LED_PORT, LED_PIN); 
        i = 0; 
      }
      i++;

      duty_cycle += 1.0f;
      if (duty_cycle > 100.0f){
        duty_cycle = 0.0f;
      }
      timer_set_pwm_duty_cycle(duty_cycle);

      start = system_get_ticks();
    }


  }

  //Never actually going to return
  return 0;
}


//check these stuff out 
//- interrupt vector table, nvic
//- systick 
