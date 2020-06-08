#ifndef DAC_HEADER_FILE
#define DAC_HEADER_FILE
#include "..//tm4c123gh6pm.h"

void ADC0_Init(void);
void Timer2_Init(unsigned long period);
unsigned long ADC0_Out(void);
#endif
