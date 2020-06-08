#include "Switch.h"
#include "Nokia5110.h"
#define PE0 0x0000001
#define PE1 0x0000002

void Switch_Init(void);
void Port_E01_Init(void);
void GPIOPortE_Handler(void);
unsigned char Get_Switch0_Value(void);
unsigned char Get_Switch1_Value(void);

unsigned char switch0_value = 0;
unsigned char switch1_value = 0;

void Switch_Init(void){
  Port_E01_Init();
}

void Port_E01_Init(void){
  volatile unsigned long PE01_delay;
	SYSCTL_RCGC2_R |= 0x00000010;   //activate clock for Port E
  PE01_delay = SYSCTL_RCGC2_R;         //allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x03;      //make PE0 and PE1 input
  GPIO_PORTE_AFSEL_R &= ~0x03;     //disable alt funct on PE0 and PE1
  GPIO_PORTE_DEN_R |= 0x03;      //enable digital on PE0 and PE1
  GPIO_PORTE_AMSEL_R &= ~0x03;     //disable analog on PE0 and 1
	GPIO_PORTE_PUR_R |= 0x03;        //enable pull up resistor
  GPIO_PORTE_IS_R &= ~0x03;     // PE01 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x03;    //     PE0 and 1 is not both edges
  GPIO_PORTE_IEV_R &= ~0x03;    //     PE0 and 1 falling edge event
  GPIO_PORTE_ICR_R = 0x03;      // clear flag 0 and 1
  GPIO_PORTE_IM_R |= 0x03;      // arm interrupt on PE0 and 1
  //IRQ: 20 interrupt number: 4
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x0000080; // (g) priority 4
  NVIC_EN0_R |= 1 << 4 ;
}

void GPIOPortE_Handler(void){
  if(GPIO_PORTE_MIS_R & PE0){
    GPIO_PORTE_ICR_R = 0x01;
    switch0_value = 1;
  }
  else if(GPIO_PORTE_MIS_R & PE1){
    GPIO_PORTE_ICR_R = 0x02;
    switch1_value = 1;
	}
}

unsigned char Get_Switch0_Value(void){
  unsigned char temp = switch0_value;
  switch0_value = 0;
  return temp;
}

unsigned char Get_Switch1_Value(void){
  unsigned char temp = switch1_value;
  switch1_value = 0;
  return temp;
}
