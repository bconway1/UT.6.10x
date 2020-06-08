#ifndef LED_HEADER_FILE
#define LEDs_HEADER_FILE
#include "..//tm4c123gh6pm.h"



void LED_Init(void);
void LED_PB4_On(void);
void LED_PB4_Off(void);
void LED_PB5_On(void);
void LED_PB5_Off(void);
void LED_Output(unsigned long value);
#endif
