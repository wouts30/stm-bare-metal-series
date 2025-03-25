#include "core/timer.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

//84_000_000
//freq = system_freq / ((prescalar-1) * (arr-1))

#define PRESCALER (84-1)
#define ARR_VALUE (1000-1)

void timer_setup(void){
    //check PWM and Timers :: Bare Metal Programming Series 3
    //https://www.youtube.com/watch?v=kHUGY05ImWU&list=PLP29wDx6QmW7HaCrRydOnxcy8QmW0SNdQ&index=5

    //84_000_000 comming in 
    rcc_periph_clock_enable(RCC_TIM2);
    //high level config
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    //setup PWM mode channel 1
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
    //enable counter for PWM output
    timer_enable_counter(TIM2);
    timer_enable_oc_output(TIM2, TIM_OC1);

    //want 1000hz and 1000 discrete counts(autoreload value) for granular control of dutycycle 
    timer_set_prescaler(TIM2, PRESCALER);
    timer_set_period(TIM2, ARR_VALUE);
}


void timer_set_pwm_duty_cycle(float duty_cycle){
    //dutycycle%  = (crr_value) / (arr_value) * 100
    //ccr = arr_value * (dutycycle%  / 100 )
    const float arr_value = (float)ARR_VALUE * (duty_cycle / 100.0f );
    //set output comnpare value that checks the counter value to determine high or low
    timer_set_oc_value(TIM2, TIM_OC1, (uint32_t)arr_value);
}