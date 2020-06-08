// DAC.h
// Header file for the 6-bit digital to analog converter
#ifndef DAC_HEADER_FILE
#define DAC_HEADER_FILE
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC
// Input: none
// Output: none
void DAC_Init(unsigned long bits);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15
// Output: none
void DAC_Out(unsigned long data);

#endif
