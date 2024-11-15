//STM32L432KC_ADC.c
//Based on code by Kevin Box and Eli Rejto
//Edited by Avery Smith
//avmith@hmc.edu
//11/12/24
//provide initialization and reading functions for ADC1

#include "STM32L432KC_RCC.h"
#include "STM32L432KC_ADC.h"

void init_ADC() {
    // initialize ADC

    GPIO // set GPIO pins to GPIO_ANALOG
    RCC -> AHB2ENR |= RCC_AHB2ENR_ADCEN; // enable clock to ADC

    ADC1_COMMON->CCR |= ADC_CCR_CKMODE_1; // select clock source in CCR

    ADC1->CR &= ~ADC_CR_DEEPPWD; //Exit Deep power 
    ADC1->CR |= ADC_CR_ADVREGEN; //Enable ADC internal voltage regulator

    ADC1 -> SQR1 |= 0xa << ADC_SQR1_L_Pos;

                                                  //////////////////////////////
                                                  //  index | channel | pin   //
    // configure for 12 conversions               //  ---------------------   //
    ADC1 -> SQR1 |= 0x5 << ADC_SQR1_SQ1_Pos;      //  0     | 5       | PA0   //
    ADC1 -> SQR1 |= 0x6 << ADC_SQR1_SQ2_Pos;      //  1     | 6       | PA1   //
    ADC1 -> SQR1 |= 0x7 << ADC_SQR1_SQ3_Pos;      //  2     | 7       | PA2   //
    ADC1 -> SQR1 |= 0x8 << ADC_SQR1_SQ4_Pos;      //  3     | 8       | PA3   //
    ADC1 -> SQR2 |= 0x9 << ADC_SQR2_SQ5_Pos;      //  4     | 9       | PA4   //
    ADC1 -> SQR2 |= 0xa << ADC_SQR2_SQ6_Pos;      //  5     | 10      | PA5   //
    ADC1 -> SQR2 |= 0xb << ADC_SQR2_SQ7_Pos;      //  6     | 11      | PA6   //
    ADC1 -> SQR2 |= 0xc << ADC_SQR2_SQ8_Pos;      //  7     | 12      | PA7   //
    ADC1 -> SQR3 |= 0xf << ADC_SQR3_SQ9_Pos;      //  8     | 15      | PB0   //
    ADC1 -> SQR3 |= 0x10 << ADC_SQR3_SQ10_Pos;    //  9     | 16      | PB1   // TODO: figure out how to use ports A and B
                                                  //////////////////////////////

    // begin calibration
    ADC1->CR &= ~(ADC_CR_ADCALDIF);
    ADC1->CR |= ADC_CR_ADCAL;

    while (ADC1->CR & ADC_CR_ADCAL); // Wait for ADCAL to read 0

    ADC1 -> CR |= ADC_CR_ADEN; // enable adc

    while(!(ADC1 -> ISR &= 1 << ADC_ISR_ADRDY_Pos)); //wait until ready
};

uint32_t read_ADC(uint16_t * adcVals) {
    // start conversion sequence
    ADC1->CR |= ADC_CR_ADSTART;
    // read ADC value
    int i;
    for (i = 0; i < 9; i++) {
        while(!(ADC1 -> ISR &= 1 << ADC_ISR_EOC_Pos));
        // read data register and store in array
        adcVals[i] = (int16_t) ADC1->DR;
    }
}