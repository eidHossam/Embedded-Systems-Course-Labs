//A program to send an SOS message
//SW1 PF4 as an input and PF3 as an output
//Pressing SW1 will start the SOS sequence which goes as follows:
//   S : Toggle LED 3 times with 0.5 second gap
// 	 O : Toggle LED 3 times with 2 second gap
//   S : Toggle LED 3 times with 0.5 second gap
//sysTickDelay50ms 5 seconds then repeat
//Pressing SW1 again will stop the sequence 

//Author : Hossam Eid
//Date : Nov, 26, 2022
#include <stdio.h>
#include <stdint.h>

//1- microcontroller ports declrations
// Constant declarations to access port registers using 
// symbolic names instead of addresses
#define SYSCTL_RCGC2_R 			(*((volatile unsigned long *)0x400FE108)) 
#define GPIO_PORTF_LOCK_R 	(*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R 		(*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R 	(*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R 	(*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_DIR_R 		(*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_ASFEL_R 	(*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DEN_R 		(*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_PUR_R 		(*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DATA_R 	(*((volatile unsigned long *)0x400253FC))

//SysTick timer declarations
#define NVIC_ST_CTRL_R 			(*((volatile unsigned long *) 0xE000E010))
#define NVIC_ST_RELOAD_R		(*((volatile unsigned long *) 0xE000E014))
#define NVIC_ST_CURRENT_R		(*((volatile unsigned long *) 0xE000E018))
	

//2 - Global Variables and Subroutines declarations

//      Global variabeles
unsigned long SW1;   //Input from switch 1 PF4

//      Subroutines declarations
void PortF_Init(void);
void sysTickDelay50ms(uint32_t counts);
void SoS(void);

int main(void)
{
	PortF_Init();   //initialize the ports
	while(1)
	{
		SW1 = GPIO_PORTF_DATA_R & 0x10;  //Take reading from SW1
		while(!SW1){										 //If SW1 is on then SOS if SW1 Off turn off SoS
			SoS();												 //Start the signal 
			sysTickDelay50ms(20);									 //wait 5 seconds 
			SW1 = GPIO_PORTF_DATA_R & 0x10;//then we read the switch again if pressed continue sending SOS , if off turn the led off
		}
		GPIO_PORTF_DATA_R &= ~0x08;			//Turn the LED Off
	}
}


void SoS(void){
	//Sending the First S signal
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R &=  ~0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R &=  ~0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(1);
	
	//Sending the O signal
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(4);
	GPIO_PORTF_DATA_R &= ~0x8;
	sysTickDelay50ms(4);
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(4);
	GPIO_PORTF_DATA_R &=  ~0x8;
	sysTickDelay50ms(4);
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(4);
	
	//Sending the last S signal
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R &=  ~0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R |= 0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R &=  ~0x8;
	sysTickDelay50ms(1);
	GPIO_PORTF_DATA_R |= 0x8;
}


void PortF_Init (void)
{volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x20; 					//1 - Enable the portF clock
	delay = SYSCTL_RCGC2_R;						//Give the clock enough time to start working
	GPIO_PORTF_LOCK_R = 0x4C4F434B;		//2 - Unlock Pf0  of PortF
	GPIO_PORTF_CR_R |= 0x1F;					//Allow changes to PF4 to PF0
	GPIO_PORTF_AMSEL_R &= 0x00;				//3-Turn off the analog mode
	GPIO_PORTF_PCTL_R &= 0x00000000;	//4-Turn of the special fundtions of the digital mode
	GPIO_PORTF_DIR_R |= 0x08;					//5-choose either input or output for each port
	GPIO_PORTF_DIR_R &= ~0x11;				//Setting PF4 and PF0 as input
	GPIO_PORTF_ASFEL_R &= 0x00000000;	//6-Turn off the alternative functions
	GPIO_PORTF_PUR_R |= 0x11;					//7-Turn of the pullup switch to work with both switches as inputs
	GPIO_PORTF_DEN_R |= 0x1f;					//8-Enable The digital port PF4 to PF0
}

void sysTickDelay50ms(uint32_t counts){
	while(counts > 0)
	{
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = 0x007FFFFF;
		NVIC_ST_CURRENT_R = 0;
		NVIC_ST_CTRL_R = 0x00000005;
		while((NVIC_ST_CTRL_R & 0x10000) == 0){}
		counts--;
	}
}






