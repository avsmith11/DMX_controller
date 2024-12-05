// STM32F401RE_TIM.c
// TIM functions
// MUST ENABLE CLOCK TO DESIRED TIMERS

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void initTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 1 us time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e6));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_micros(TIM_TypeDef * TIMx, uint32_t us){
  TIMx->ARR = us;// Set timer
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT &= 0; // Reset counts

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = ms * 1000;// Set timer
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void reset_timer(TIM_TypeDef * TIMx) {
  TIMx->ARR = 0XFFFF;   // Set ARR to max
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count  
}

uint16_t read_timer(TIM_TypeDef * TIMx) {
  TIMx->CR1 &= ~1;
  return TIMx->CNT; // read timer value
}