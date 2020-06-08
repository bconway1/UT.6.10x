// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

#define CM_PER_ADC_SAMPLE_VALUE 0.48828

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata = 0;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.
// Overflow and dropout should be considered
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  unsigned long result;
	sample = sample & 0xFFFFFF;
	result = sample * CM_PER_ADC_SAMPLE_VALUE;
	return result;  // replace this line with real code
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x00000007;
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){
	ADCdata = ADC0_In();
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"
//   31 to "0.031 cm"
//  102 to "0.102 cm"
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
	unsigned long next = n;
	unsigned char r;
	int i;
	String[5] = ' ';
	String[6] = 'c';
	String[7] = 'm';
	if(n > 9999){
		for(i = 0; i <= 4; i++){
			if(i == 1) {
				String[i] = '.';
				continue;
			}
			String[i] = '*';
		}
		return;
	}

	for(i = 4; i >= 0; i--){
		if(i == 1){
			String[i] = '.';
			continue;
		}
		r = next % 10;
		next /= 10;
		String[i] = r + 0x30;

	}
}

// main1 is a simple main program allowing you to debug the ADC interface
// int main1(void){
//   TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
//   ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
//   EnableInterrupts();
//   while(1){
//     ADCdata = ADC0_In();
//   }
// }
// once the ADC is operational, you can use main2 to debug the convert to distance
// int main2(void){
//   TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
//   ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
//   Nokia5110_Init();             // initialize Nokia5110 LCD
//   EnableInterrupts();
//   while(1){
//     ADCdata = ADC0_In();
//     Nokia5110_SetCursor(0, 0);
//     Distance = Convert(ADCdata);
//     UART_ConvertDistance(Distance); // from Lab 11
//     Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
//   }
// }
// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void){
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();// initialize ADC0, channel 1, sequencer 3
// initialize Nokia5110 LCD (optional)
	Nokia5110_Init();
	SysTick_Init(1999999);// initialize SysTick for 40 Hz interrupts
// initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize
	
  EnableInterrupts();
// print a welcome message  (optional)
	Nokia5110_OutString("Hello! Move the slide pot to see the distance change!");
  while(1){
// read mailbox
		Nokia5110_Clear();
		Distance = Convert(ADCdata);
		UART_ConvertDistance(Distance);
		Nokia5110_OutString(String);
// output to Nokia5110 LCD (optional)
  }
}
