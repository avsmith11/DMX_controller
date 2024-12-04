//STM32L432KC_ADC.c
//Based on code by Kevin Box and Eli Rejto
//Edited by Avery Smith
//avmith@hmc.edu
//11/12/24
//provide initialization and reading functions for ADC1

#include "STM32L432KC_RCC.h"
#include "STM32L432KC_ADC.h"
#include "STM32L432KC_GPIO.h"

void initADC() {
    // initialize ADC

    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);
        
    pinMode(PA0, GPIO_ANALOG);
    pinMode(PA1, GPIO_ANALOG);
    pinMode(PA2, GPIO_ANALOG);
    pinMode(PA3, GPIO_ANALOG);
    pinMode(PA4, GPIO_ANALOG);
    pinMode(PA5, GPIO_ANALOG);
    pinMode(PA6, GPIO_ANALOG);
    pinMode(PA7, GPIO_ANALOG);
    pinMode(PB0, GPIO_ANALOG);
    pinMode(PB1, GPIO_ANALOG);

    RCC -> AHB2ENR |= RCC_AHB2ENR_ADCEN; // enable clock to ADC

    ADC1_COMMON->CCR |= ADC_CCR_CKMODE_1; // select clock source in CCR

    ADC1->CR &= ~ADC_CR_DEEPPWD; //Exit Deep power 
    ADC1->CR |= ADC_CR_ADVREGEN; //Enable ADC internal voltage regulator

    ADC1 -> SQR1 |= 0xb << ADC_SQR1_L_Pos;

                                                  //////////////////////////////
                                                  //  index | channel | pin   //
    // configure for 10 conversions               //  ---------------------   //
    ADC1 -> SQR1 |= (0x5 << ADC_SQR1_SQ1_Pos);    //  0     | 5       | PA0   //
    ADC1 -> SQR1 |= (0x6 << ADC_SQR1_SQ2_Pos);    //  1     | 6       | PA1   //
    ADC1 -> SQR1 |= (0x7 << ADC_SQR1_SQ3_Pos);    //  2     | 7       | PA2   //
    ADC1 -> SQR1 |= (0x8 << ADC_SQR1_SQ4_Pos);    //  3     | 8       | PA3   //
    ADC1 -> SQR2 |= (0x9 << ADC_SQR2_SQ5_Pos);    //  4     | 9       | PA4   //
    ADC1 -> SQR2 |= (0xa << ADC_SQR2_SQ6_Pos);    //  5     | 10      | PA5   //
    ADC1 -> SQR2 |= (0xb << ADC_SQR2_SQ7_Pos);    //  6     | 11      | PA6   //
    ADC1 -> SQR2 |= (0xc << ADC_SQR2_SQ8_Pos);    //  7     | 12      | PA7   //
    ADC1 -> SQR2 |= (0xf << ADC_SQR2_SQ9_Pos);    //  8     | 15      | PB0   //
    ADC1 -> SQR3 |= (0x10 << ADC_SQR3_SQ10_Pos);  //  9     | 16      | PB1   //
                                                  //////////////////////////////

    // set sample time to maximum
    ADC1  -> SMPR1 |= (0b111 << ADC_SMPR1_SMP5_Pos);
    ADC1  -> SMPR1 |= (0b111 << ADC_SMPR1_SMP6_Pos);
    ADC1  -> SMPR1 |= (0b111 << ADC_SMPR1_SMP7_Pos);
    ADC1  -> SMPR1 |= (0b111 << ADC_SMPR1_SMP8_Pos);
    ADC1  -> SMPR1 |= (0b111 << ADC_SMPR1_SMP9_Pos);
    ADC1  -> SMPR2 |= (0b111 << ADC_SMPR2_SMP10_Pos);
    ADC1  -> SMPR2 |= (0b111 << ADC_SMPR2_SMP11_Pos);
    ADC1  -> SMPR2 |= (0b111 << ADC_SMPR2_SMP12_Pos);
    ADC1  -> SMPR2 |= (0b111 << ADC_SMPR2_SMP15_Pos);
    ADC1  -> SMPR2 |= (0b111 << ADC_SMPR2_SMP16_Pos);



    // begin calibration
    ADC1->CR &= ~(ADC_CR_ADCALDIF); // set to single-ended mode
    ADC1->CR |= ADC_CR_ADCAL;

    while (ADC1->CR & ADC_CR_ADCAL); // Wait for ADCAL to read 0

    ADC1 -> CR |= ADC_CR_ADEN; // enable adc

    while(!(ADC1 -> ISR &= 1 << ADC_ISR_ADRDY_Pos)); //wait until ready
    return;
};

void readADC(volatile uint16_t * adcVals) {
    // start conversion sequence
    ADC1->CR |= ADC_CR_ADSTART;
    // read ADC values
    int i = 0;
    ADC1 -> ISR |= ADC_ISR_EOS; // clear EOS bit
    while (!(ADC1 -> ISR & 1 << ADC_ISR_EOS_Pos)) {
        while(!(ADC1 -> ISR & 1 << ADC_ISR_EOC_Pos)); // wait for end of conversion (EOC)

        ADC1 -> ISR |= ADC_ISR_EOC; // clear EOC by writing 1
        // read data register and store in array
        adcVals[i] = (int16_t) ADC1->DR;

        i++ ; // increment adcVals index

    }
    return;
}

uint16_t readCH6() {
    // start conversion sequence
    ADC1->CR |= ADC_CR_ADSTART;
    while(!(ADC1 -> ISR & 1 << ADC_ISR_EOC_Pos));
    ADC1 -> ISR |= ADC_ISR_EOC;
    return (int16_t) ADC1->DR;

}