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
void sendSPItoFPGA(char value1, char value2);
char intToAsciiChar(int digit);
void averageADC(uint16_t * averagedValues);

void main(){
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
    volatile uint16_t averagedValues[10];
    volatile char lastParamValues[10];
    volatile char activeParam;
    volatile char activeValue;
    
    //testADC();

    while(1){
      //sendSPItoFPGA('0', 0x45);
      spiSend(0b10111001);
      delay_micros(TIM6, 1);
      //for (int i=0; i<100; i++) delay_millis(TIM6, 20);
      //sendSPItoFPGA('5', 25);
      //for (int i=0; i<100; i++) delay_millis(TIM6, 20);
    }


    while(1){
        // shift paramValues to lastParamValues
        // read ADC
        // cast 12-bit range to 8 bits
        // send parameters over DMX
        // find what value last changed
        // send parameters to FPGA over SPI
        // wait 100ms.

        averageADC(averagedValues);

        paramValues[0] = uint12_to_char(averagedValues[0]);
        paramValues[1] = uint12_to_char(averagedValues[1]);
        paramValues[2] = uint12_to_char(averagedValues[2]);
        paramValues[3] = uint12_to_char(averagedValues[3]);
        paramValues[4] = uint12_to_char(averagedValues[4]);
        paramValues[5] = uint12_to_char(averagedValues[5]);
        paramValues[6] = 0;
        paramValues[7] = 0;
        paramValues[8] = 0;


        

        
        for (int i = 0; i < 10; i++) {
            printf("param %u: %u\n", i, paramValues[i]);

            if ((lastParamValues[i] != paramValues[i]) & (i<8)){  // check which value changed and set activeValue, activeParam
                activeValue = paramValues[i];
                activeParam = i;
            }
            lastParamValues[i] = paramValues[i]; // send param values to last param values
        }

        sendDMX(&paramValues);

        sendSPItoFPGA(activeValue, activeParam); // TODO: change to ascii
    }
    
}

void averageADC(uint16_t averagedValues[10]) {

    volatile uint16_t adcValues[10];

    uint32_t sum[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // use uint32 to prevent overflow
    
    // read adc 10 times
    for (int j = 0; j < 20; j++) {
        
        readADC(&adcValues);

        // add adc values to sum array
        for (int i = 0; i < 10; i++) {
            sum[i] += adcValues[i];
        }
    }
    // divide each sum element by 10
    for (int i = 0; i < 10; i++) {
        averagedValues[i] = sum[i]/20;
    }
    
}

void testADC() {
    volatile uint16_t adcValues[10];  // Array to store 10 ADC values
    while (1) {
      // Call the readADC function, passing the address of the array
      //for (int i = 0; i < 100; i++) delay_millis(TIM6, 10);
      averageADC(adcValues);

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
      pinMode(PA9, GPIO_ALT); // set PA9 to tx
      sendChar(USART1, 0x0); // send Null start frame
      sendString(USART1, paramValues, 10); // send data
      //delay_millis(TIM6, 40);

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

void sendSPItoFPGA(char value1, char value2) {
    digitalWrite(PB4, 1); // set Load high
    spiSend(value1); // send first byte
    spiSend(value2); // send second byte
    digitalWrite(PB4, 0); // set Load low
}

char intToAsciiChar(int digit) {
    // another excellent helper function from chatGPT
    if (digit >= 0 && digit <= 9) {
        return '0' + digit;
    } else {
        return '\0'; // Return null character for invalid input
    }
}