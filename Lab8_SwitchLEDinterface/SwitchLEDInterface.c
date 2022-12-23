// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
//      Jon Valvano and Ramesh Yerraballi
//Written By: Hossam Eid

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include <stdint.h>

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
	
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count);
void Port_Init(void);
// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  EnableInterrupts();           // enable interrupts for the grader
	Port_Init();
	while(1){
    Delay100ms(1);
		
		if(GPIO_PORTE_DATA_R & 0x01)
		{
				GPIO_PORTE_DATA_R ^= 0x02;
		}else{
			GPIO_PORTE_DATA_R 	|= 0x02;
		}
  }
  
}

void Port_Init(void)
{unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_AMSEL_R 	&= 0x00000000;
	GPIO_PORTE_PCTL_R 	&= 0x00000000;
	GPIO_PORTE_AFSEL_R 	&= 0x00000000;
	GPIO_PORTE_DIR_R    |= 0x02;
	GPIO_PORTE_DIR_R    &= ~0x01;
	GPIO_PORTE_PUR_R 		&= ~0x01;
	GPIO_PORTE_DEN_R 		|= 0x03;
	GPIO_PORTE_DATA_R 	|= 0x02;
}

void Delay100ms(uint32_t count)
{
	while(count > 0)
	{
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = 0x007FFFFF;
		NVIC_ST_CURRENT_R = 0;
		NVIC_ST_CTRL_R |= 0x05;
		while((NVIC_ST_CTRL_R & 0x10000) == 0)
		{
		}
		count--;
	}
}	
	
	