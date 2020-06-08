#include "DAC.h"

void DAC_Init(unsigned long bits);
void DAC_Out(unsigned long data);

// **************DAC_Init*********************
// Initialize 4-bit DAC
// Input: none
// Output: none
void DAC_Init(unsigned long bits){
  int DAC_delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; //activate clock on port B
	DAC_delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R |= bits;
	GPIO_PORTB_AFSEL_R &= ~bits;
	GPIO_PORTB_PCTL_R &= ~0xFFFFFF;
	GPIO_PORTB_AMSEL_R &= ~bits;
	GPIO_PORTB_DEN_R |= bits;
	GPIO_PORTB_DR8R_R |= bits;
	GPIO_PORTB_DATA_R = 0x00; //starting output is zero
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15
// Output: none
void DAC_Out(unsigned long data){
  data = data & 0x0000000F; // make sure we only get bottom six bits
	GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0x0F) | (0x0F & data);
}
