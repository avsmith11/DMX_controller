// main.c
// Avery Smith
// avsmith@hmc.edu
// 11/18/24
// min function for the DMX controller project. Read 8 analog signals from linear potentiometers "sliders" and 
// output 8 or more paramter values over UART using DMX512 specifications

#include "../lib/STM32L432KC.h"

void main(){
    // testing ADC functionality

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

    initADC();

    while (1) {
        printf("Value: %u\n", readCH6());
    };

    //uint16_t adcValues[9];  // Array to store 9 ADC values
    //while (1) {
    //  // Call the readADC function, passing the address of the array
    //  readADC(adcValues);

    //  // Print the ADC values
    //  for (int i = 0; i < 9; i++) {
    //      printf("ADC Value %d: %u\n", i, adcValues[i]);
    //  };
    //};
}