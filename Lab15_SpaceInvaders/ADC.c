#include "ADC.h"
#include "..//tm4c123gh6pm.h"
void ADC0_Init(void);
void Timer2_Init(unsigned long period);
void Timer2A_Handler(void);
unsigned long ADC0_In(void);
unsigned long ADC0_Out(void);


volatile unsigned long ADC_sample = 0;

void ADC0_Init(void){
  volatile unsigned long ADC_delay;
	SYSCTL_RCGC2_R |= 0x00000010;   //activate clock for Port E
  ADC_delay = SYSCTL_RCGC2_R;         //allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x04;      //make PE2 input for ADC channel 1
  GPIO_PORTE_AFSEL_R |= 0x04;     //enable alt funct on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      //disable digital on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     //enable analog on PE2
	SYSCTL_RCGC0_R |= 0x00010000;   //activate ADC0
  ADC_delay = SYSCTL_RCGC2_R;        	//wait to settle
	SYSCTL_RCGC0_R &= ~0x00000300;  //config ADC0 for 125K
	ADC0_SSPRI_R = 0x0123;          //Seq 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;        //disable seq 3 for init
	ADC0_EMUX_R &= ~0xF000;         //seq 3 is software triggered in systick isr
	ADC0_SSMUX3_R &= ~0x000F;       //clear SS3 field
	ADC0_SSMUX3_R += 1;             //set channel Ain1 -> PE2
	ADC0_SSCTL3_R = 0x0006;         //clear TS0 D0 bits, set IE0 END0 bits
	ADC0_ACTSS_R |= 0x0008;         //enable seq3
}

void Timer2_Init(unsigned long period){
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;

  TIMER2_CTL_R = 0x00000000;    //  disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    //  configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   //  configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    //  reload value
  TIMER2_TAPR_R = 0;            //  bus clock resolution
  TIMER2_ICR_R = 0x00000001;    //  clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    //  arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  ADC_sample = ADC0_In();

}

//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){
  unsigned long sample;
	ADC0_PSSI_R = 0x0008;            //take sample on seq3

	while((ADC0_RIS_R & 0x08) == 0){};   //wait for a to d convcersion

	sample = ADC0_SSFIFO3_R & 0xFFF;   //read 12 bit result
	ADC0_ISC_R = 0x0008;             //ack adc complete
	return sample;
}

unsigned long ADC0_Out(void){
  unsigned long temp = ADC_sample;
  return temp;
}
