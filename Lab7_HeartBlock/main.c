// 0.Documentation Section 
// HeartBlock, main.c

// Runs on LM4F120 or TM4C123 LaunchPad
// Input : Artial sensor SW1 (PF4)
// Output : The Ready signal PF3 Green LED ,  The Ventricular trigger (VT) PF1 Red LED

// Repeat this sequence over and over again
// 1 - Initially the led is green which means the paceMaker is ready 
// 2 - Wait untill The artial sensor is 0 which tells us that the artial began to contract, which means SW1 is pressed
// 3 - Make the Ready signal go low, Turn of the Green LED
// 4 - Wait 10ms to eliminate the switch bounce
// 5 - wait until SW1 is released goes to (1) that means that the artial is done contracting
// 6 - delay for 250ms
// 7 - Turn the LED red, VT signal high
// 8 - Delay for 250ms
// 9 - Turn the LED off, VT signal low

// Date : Nov, 28, 2022
// Author : Hossam Eid

#include <stdio.h>
#include "TExaS.h"
#include <stdint.h>

// 1) Pre-processor Directives Section

//Constant declarations to access the port registers usign symbolic names instead of addresses
#define SYSCTL_RCGC2_R 			(*((volatile unsigned long *) 0x400FE108))
#define GPIO_PORTF_LOCK_R 	(*((volatile unsigned long *) 0x40025520))
#define GPIO_PORTF_CR_R			(*((volatile unsigned long *) 0x40025524))
#define GPIO_PORTF_AMSEL_R 	(*((volatile unsigned long *) 0x40025528))
#define GPIO_PORTF_PCTL_R 	(*((volatile unsigned long *) 0x4002552C))
#define GPIO_PORTF_DIR_R 		(*((volatile unsigned long *) 0x40025400))
#define GPIO_PORTF_AFSEL_R 	(*((volatile unsigned long *) 0x40025420))
#define GPIO_PORTF_PUR_R 		(*((volatile unsigned long *) 0x40025510))
#define GPIO_PORTF_DEN_R 		(*((volatile unsigned long *) 0x4002551C))
#define GPIO_PORTF_DATA_R 	(*((volatile unsigned long *) 0x400253FC))

//SysTick timer declarations
#define NVIC_ST_CTRL_R 			(*((volatile unsigned long *) 0xE000E010))
#define NVIC_ST_RELOAD_R		(*((volatile unsigned long *) 0xE000E014))
#define NVIC_ST_CURRENT_R		(*((volatile unsigned long *) 0xE000E018))
	

// 2) Declarations Section

// 	Global Variables
#define AS  		(*((volatile unsigned long *)0x40025040))     //Output (SW1)  PF4
#define Ready  	(*((volatile unsigned long *)0x40025020))			//Input  				PF3
#define VT 			(*((volatile unsigned long *)0x40025008))			//Input					PF1

unsigned long SW1;
unsigned long LED;

// Subroutines declarations
void Port_Init(void);
void setReady(void);
void clearReady(void);
void waitForASLow(void);
void waitForASHigh(void);
void setVT(void);
void clearVT(void);
void sysTickDelay10ms(uint32_t counts);
void sysTickDelay50ms(uint32_t counts);

// 3) Main : Where our program starts excuting
int main(void)
{
	TExaS_Init(SW_PIN_PF40, LED_PIN_PF31,ScopeOn);  // activate grader and set system clock to 80 MHz
  Port_Init();                            // Init port PF4 PF3 PF1    
  EnableInterrupts();
	while(1)
	{	
		setReady();
		waitForASLow();
		clearReady();
		sysTickDelay10ms(1);
		waitForASHigh();
		sysTickDelay50ms(5);
		setVT();
		sysTickDelay50ms(5);
		clearVT();
	}
}


// 4) Subroutines Implementation
void Port_Init(void){
	volatile unsigned long delay;
	
		SYSCTL_RCGC2_R |= 0x20;								//Enable the PORTF clock
		delay = SYSCTL_RCGC2_R;								//Give the clock enough time to start
		GPIO_PORTF_AFSEL_R  &= 0x00;					//Turn off Analog mode for the pins
		GPIO_PORTF_PCTL_R 	&= 0x00000000;		//Turn off the special Digital mode
		GPIO_PORTF_AMSEL_R 	&= 0x0;						//Turn off the alternate analog functions
		GPIO_PORTF_DIR_R  	|= 0x0E;					//Set PF3 , PF1 as inputs
		GPIO_PORTF_DIR_R 		&= ~0x10;					//Set PF4 as input
		GPIO_PORTF_PUR_R 		|= 0x10;					//Enable pullup resistor for PF4
		GPIO_PORTF_DEN_R 		|= 0x1E;					//Enable Digital pins PF4 to PF0
}






void setReady(void){
	GPIO_PORTF_DATA_R |= 0x8;											//Ready signal initially is on
}






void clearReady(void){
	GPIO_PORTF_DATA_R &= ~0x8;									//Turn off the ready signal
}




void waitForASLow(void){
	do{
		SW1 = GPIO_PORTF_DATA_R & 0x10;
	}while(SW1);
	
}




void waitForASHigh(void){
	do{
		SW1 = GPIO_PORTF_DATA_R & 0x10;
	}while(!SW1);
}	




void setVT(void){
	GPIO_PORTF_DATA_R |= 0x2;
}




void clearVT(void){
	GPIO_PORTF_DATA_R &= ~0x2;
}	

	


void sysTickDelay50ms(uint32_t counts){
	while(counts > 0)
	{
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = 0x003D0900;
		NVIC_ST_CURRENT_R = 0;
		NVIC_ST_CTRL_R = 0x00000005;
		while((NVIC_ST_CTRL_R & 0x10000) == 0){}
		counts--;
	}
}



void sysTickDelay10ms(uint32_t counts){
	while(counts > 0)
	{
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = 0x000C3500;
		NVIC_ST_CURRENT_R = 0;
		NVIC_ST_CTRL_R = 0x00000005;
		while((NVIC_ST_CTRL_R & 0x10000) == 0){}
		counts--;
	}
}