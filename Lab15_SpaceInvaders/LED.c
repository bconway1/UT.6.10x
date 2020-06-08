#include "LED.h"

void LED_Init(void);
void LED_PB4_On(void);
void LED_PB4_Off(void);
void LED_PB5_On(void);
void LED_PB5_Off(void);

void LED_Init(void){
  int LED_delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; //activate clock on port B
	LED_delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R |= 0x30; //PB4 and PB5 direction output
	GPIO_PORTB_AFSEL_R &= ~0x30;
	GPIO_PORTB_PCTL_R &= ~0xFFFFFF;
	GPIO_PORTB_AMSEL_R &= ~0x30;
	GPIO_PORTB_DEN_R |= 0x30;
	GPIO_PORTB_DATA_R = 0x00; //starting output is zero
}

void LED_Output(unsigned long value){
  GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0x30) | (value & 0x30);
}

void LED_PB4_On(void){
  GPIO_PORTB_DATA_R |= 0x10;
}
void LED_PB4_Off(void){
  GPIO_PORTB_DATA_R &= ~0x10;
}

void LED_PB5_On(void){
  GPIO_PORTB_DATA_R |= 0x20;
}
void LED_PB5_Off(void){
  GPIO_PORTB_DATA_R &= ~0x20;
}
