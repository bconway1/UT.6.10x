// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SENSOR  						(*((volatile unsigned long *)0x4002401C))
#define STREET_LIGHT   			(*((volatile unsigned long *)0x400050FC))
#define PED_LIGHT						(*((volatile unsigned long *)0x40025028))
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
	
// ***** 2. Global Declarations Section *****
unsigned long delay;

struct State {
  unsigned long Out;  // 6-bit pattern to output
	unsigned long Walk;  // 2 & 0 bit field to ped sign
  unsigned long Time; // delay in 10ms units 
  unsigned long Next[8];
}; 
typedef const struct State state_t;

#define goS           0 
#define waitS         1 
#define goW           2 
#define waitW         3 
#define goPed         4 
#define hurryPed0     5 
#define hurryPed1     6 
#define hurryPed2     7
#define hurryPed3     8

#define OFF           0x0
#define RED           0x2
#define GREEN         0x8

state_t FSM[9] = {
  {0x21, RED, 50, {goS, waitS, goS, waitS, waitS, waitS, waitS, waitS}}, 
 
  {0x22, RED, 50, {goW, goW, goS, goW, hurryPed1, goW, goPed, goW}},
 
  {0x0C, RED, 50, {waitW, goW, waitW, waitW, waitW, waitW, waitW, waitW}},

  {0x14, RED, 50, {goS, goW, goS, goS, hurryPed1, goPed, goS, goPed}},
 
  {0x24, GREEN, 50, {hurryPed2, hurryPed2, hurryPed2, hurryPed2, goPed, hurryPed2, hurryPed2, hurryPed1}}, 
 
  {0x24, RED, 50, {hurryPed1, hurryPed1, hurryPed1, hurryPed1, hurryPed1, hurryPed1, hurryPed1, goS}},
 
  {0x24, OFF, 50, {goS, goW, goS, goW, goPed, goW, goS, hurryPed2}},
 
  {0x24, RED, 50, {hurryPed3, hurryPed3, hurryPed3, hurryPed3, hurryPed3, hurryPed3, hurryPed3, hurryPed3}},
 
  {0x24, OFF, 50, {hurryPed0, hurryPed0, hurryPed0, hurryPed0, goPed, hurryPed0, hurryPed0, hurryPed0}}
						
};

unsigned long Input;
unsigned long S;
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
void Ports_Init(void);
// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
 
  
  EnableInterrupts();
	
	PLL_Init();
	SysTick_Init();
	Ports_Init();
	S = goS;
  while(1){
    STREET_LIGHT = FSM[S].Out;  // set lights
		PED_LIGHT = FSM[S].Walk;  // set walk lights
    SysTick_Wait10ms(FSM[S].Time);
    Input = SENSOR;     // read sensors
    S = FSM[S].Next[Input]; 
  }
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}



void Ports_Init(void){
	SYSCTL_RCGC2_R |= 0x32;      // 1) F E B
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
	
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
	GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
	GPIO_PORTF_AMSEL_R &= ~0x0A; // 3) disable analog function on PF3 & PF1
	
	
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
	GPIO_PORTB_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
	GPIO_PORTF_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
	
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
	GPIO_PORTB_DIR_R |= 0x3F;   // 5) outputs on PB5-0
	GPIO_PORTF_DIR_R |= 0x0A;   // 5) outputs on PF3 & PF1
	
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE1-0
	GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
	GPIO_PORTF_AFSEL_R &= ~0x0A; // 6) regular function on PF3 & PF1
	
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE1-0
	GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	GPIO_PORTF_DEN_R |= 0x0A;    // 7) enable digital on PF3 & PF1
	
  
}
	
