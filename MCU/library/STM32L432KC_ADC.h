// STM32L432KC_ADC.h
// Avery Smith
// avsmith@hmc.edu
// 11/12/2024
// header for ADC functions

#ifndef STM32L4_ADC_H
#define STM32L4_ADC_H

#include <stdint.h>
#include <stm32l432xx.h>

void adcEnable();
uint32_t readADC(uint32_t adc_port);
#endif