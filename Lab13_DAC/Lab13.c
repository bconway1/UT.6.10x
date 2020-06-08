// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"

#define NUM_OF_NOTES 4
//unsigned long note_freq[NUM_OF_NOTES] = {102042, 121349, 136209, 152890};

unsigned long note_freq[NUM_OF_NOTES] = {152890, 136210, 121349, 102042};

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int key_value_shift(unsigned long key_value);

int main(void){ // Real Lab13
	unsigned long key_pressed;
	int note_index = -1;
	int past_note_index = -1;
	// for the real board grader to work
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done

	while(1){
// input from keys to select tone
		key_pressed = Piano_In();
		note_index = key_value_shift(key_pressed);
		if( (note_index != -1) && (note_index != past_note_index) ){
			Sound_Tone(note_freq[note_index]);
			past_note_index = note_index;
		}
		if( note_index == -1){
			Sound_Off();
			past_note_index = -1;
		}
  }

}



// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) {
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}

int key_value_shift(unsigned long key_value){
	int i;
	int num_of_keys_pressed = 0;
	int key = -1;
	key_value = key_value & 0x0F;
	for(i = 0; i < NUM_OF_NOTES; i++){
		if((key_value & 0x01) == 0x01){
			key = i;
			num_of_keys_pressed++;
		}
		key_value = key_value >> 1;
	}
	if( (num_of_keys_pressed == 0) || (num_of_keys_pressed > 1) ){
		key = -1;
	}
	return key;
}
