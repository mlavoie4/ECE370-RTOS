/*
 * RTOS.c
 *
 * Created: 9/18/2019 9:25:19 AM
 * Author : mlavoie4
 */ 

#define F_CPU 16000000
#define tStackSize = 60;
#define timeDel = 0;

#include "include/FreeRTOS.h"

static uint16_t ticks = 0;
uint16_t segtab[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
uint16_t Val = 0x0000;
uint16_t strobereg;

void main()
{
    DDRB|= 0xFF;
    DDRD = 0xFF;
    DDRE = 0x00;
	
	TCNT1 = timeDel;
	TCCR1A = 0x00;			//normal mode
    TCCR1B = (0b001<<CS10);	//no prescaler
    TCCR1C = 0x00;
	
	sei();
	
    /*while (1) 
	{
		if((PINE&0x01)==0)
			Val--;
		else if((PINE&0x02)==0)
			Val++;
		else
			Val=Val;
	}*/
	
	while(1)
	{
		vTaskStartScheduler();
	}
}

ISR(TIMER1_OVF_vect)
{
	RTOS();					//call RTOS atomic task
	TCNT1 = timeDel;
}

void RTOS()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint8_t ledState;
		//master variable, or all task states to see if any are actively running
		//ticks++;
		
		//logic for task enables
		//if ((ticks % 1024 == 0) && ledState == 0) ledState = ledStrobe();
		
		BaseType_t xReturned;
		TaskHandle_t xHandle = NULL;
		
		xReturned = xTaskCreate(ledStrobe, "LED", tStackSize, NULL, 1, NULL);
	}
}

/* Task States:
	Ready		0
	Run			1
	Complete	2
	Dead		3
*/

void ledStrobe(void * pvParameters)
{
	sei();
		
	uint8_t D=0;
	
	switch(strobereg)
	{
		case 3:
			D = (Val)%16; break;
		case 2:
			D = (Val/16)%16; break;
		case 1:
			D = (Val/256)%16; break;
		case 0:
			D = (Val/4096)%16; break;
	}
	
	PORTB = 0;					//turns off the display
	PORTD = segtab[D];
	PORTB = 1<<strobereg;
	strobereg = (strobereg + 1) & 3;
	return 2;
	
	cli();
	
	vTaskDelete(NULL);
}
