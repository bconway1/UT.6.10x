// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

#define NUM_OF_SAMPLES 30
#define NUM_OF_NOTES 4

int sample_index = 0; 
const unsigned long SineWave[NUM_OF_SAMPLES] = {36, 40, 44, 48, 52, 56, 60, 64, 60, 56, 52, 48, 44, 40, 36, 
																								32, 28, 24, 20, 16, 12, 8, 4, 8, 12, 16, 20, 24, 28, 32};  

//Note_Values are calculated by converting Hz -> nanosec which is then divided by 12.5(system at 80MHz)																								
																		

																				
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  unsigned long initial_reload = 0;
	DAC_Init();
	NVIC_ST_CTRL_R = 0;         // disable sysTick for init
	NVIC_ST_RELOAD_R = initial_reload; //set init reload value
	NVIC_ST_CURRENT_R = 0; //clear current
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF) | 0x20000000; //priority 1
	NVIC_ST_CTRL_R = 0x0007; //enable systick, source core clock, enable interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_RELOAD_R = (period / NUM_OF_SAMPLES) - 1; //sine wave has 30 elements to output from DAC to create wave
	NVIC_ST_CURRENT_R = 0; //restarts countdown
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	NVIC_ST_RELOAD_R = 0;
	DAC_Out(0);
	sample_index = 0;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
  
	DAC_Out((SineWave[sample_index]) - 1); //DAC can only handle up to value 63
	sample_index = (sample_index + 1) % NUM_OF_SAMPLES;
}



