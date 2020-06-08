#ifndef AUDIO_HEADER_FILE
#define AUDIO_HEADER_FILE
#include "..//tm4c123gh6pm.h"

void Audio_Init(unsigned long DAC_bits, unsigned long Timer0_period);

void Audio_Play(const unsigned char* sound, unsigned long count);

void Audio_Shoot(void);
void Audio_Killed(void);
void Audio_Explosion(void);
void Audio_Fastinvader1(void);
void Audio_Fastinvader2(void);
void Audio_Fastinvader3(void);
void Audio_Fastinvader4(void);
void Audio_Highpitch(void);
#endif
