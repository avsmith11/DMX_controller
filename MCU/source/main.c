// main.c
// Avery Smith
// avsmith@hmc.edu
// 11/18/24
// min function for the DMX controller project. Read 8 analog signals from linear potentiometers "sliders" and 
// output 8 or more paramter values over UART using DMX512 specifications

#include "../lib/STM32L432KC.h"
#include <string.h>


// PIN ASSIGNMENTS
//  PA0 |   ADC0
//  PA1 |   ADC1
//  PA2 |   ADC2
//  PA3 |   ADC3
//  PA4 |   ADC4
//  PA5 |   ADC5
//  PA6 |   ADC6
//  PA7 |   ADC7
//  PB0 |   ADC8
//  PB1 |   ADC9
//  PA9 |   UART1 Tx
//  PB5 |   SPI1 PICO
//  PB3 |   SPI1 SCLK
//  PB4 |   GPIO - LOAD

// Protoype functions
void testADC();
void sendDMX(volatile char * paramValues);
char uint12_to_char(uint16_t value);
void sendSPItoMCU(char value1, char value2)

void main(){
    // testing ADC functionality
    configureFlash();
    configureClock();
    gpioEnable(GPIO_PORT_A);
    gpioEnable(GPIO_PORT_B);

    initADC();
    USART_TypeDef * USART = initUSART(USART1_ID, 250000); // initialize USART1 with 250kbit/s baudrate
    initSPI(1, 0, 0);
    pinMode(PB4, GPIO_OUTPUT); // set PB4 to output for Load signal
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN; // enable clock to timer 6
    initTIM(TIM6);


    volatile char paramValues[10];
    volatile uint16_t adcValues[10];
    volatile char lastParamValues[10];
    volatile char activeParam;
    volatile char activeValue;


    while(1){
        // shift paramValues to lastParamValues
        // read ADC
        // cast 12-bit range to 8 bits
        // send parameters over DMX
        // find what value last changed
        // send parameters to FPGA over SPI
        // wait 100ms.

        readADC(&adcValues);
        
        for (int i = 0; i < 10; i++) {
            paramValues[i] = uint12_to_char(adcValues[i]);
            printf("adc %u: %u -> %u\n", i+1, adcValues[i], paramValues[i]);

            if ((i > 5) & (i < 6 ) & (lastParamValues[i] != paramValues[i])){  // check which value changed and set activeValue, activeParam TODO: remove case on i once full implementation complete
                activeValue = paramValues[i];
                activeParam = i+1;
            }
        }
        sendDMX(&paramValues);
        sendSPItoMCU(activeValue, activeParam)
        delay_millis(TIM6, 100);
    }
    
}

void testADC() {
    volatile uint16_t adcValues[10];  // Array to store 10 ADC values
    while (1) {
      // Call the readADC function, passing the address of the array
      readADC(&adcValues);

      // Print the ADC values
      for (int i = 0; i < 10; i++) {
          printf("ADC Value %d: %u\n", i, adcValues[i]);
      }
    }
}

void sendDMX(volatile char * paramValues) {
      // frame and transmit parameter values in accordance with DMX standard

      while(!(USART1->ISR & USART_ISR_TXE)); // wait for empty transmit buffer
      pinMode(PA9, GPIO_OUTPUT); // set PA9 mode to output
      digitalWrite(PA9, 0); // set PA9 low
      delay_micros(TIM6, 176); // wait 176us
      digitalWrite(PA9, 1); //set PA9 high
      delay_micros(TIM6, 12); //wait 12us
      pinMode(PA9, GPIO_ALT); // set PA9 mode to analog
      sendChar(USART1, 0x0); // send Null start frame
      sendString(USART1, paramValues, 10); // send data
}

char uint12_to_char(uint16_t value) {
    // credit to chatgpt for this
    // Ensure value is within the 12-bit range (0 to 4095)
    if (value > 4095) {
        value = 4095; // Clamp to the maximum 12-bit value
    }

    // Perform the scaling
    return (char)((value * 255) / 4095);
}

void sendSPItoMCU(char value1, char value2) {
    digitalWrite(PB4, 1); // set Load high
    spiSend(value1); // send first byte
    spiSend(value2); // send second byte
    digitalWrite(PB4, 0); // set Load low
}