/*
 * RTOS.c
 *
 * Created: 9/18/2019 9:25:19 AM
 * Author : mlavoie4
 */ 

#define F_CPU 16000000
#define timeDel = 0;

#include "include/FreeRTOS.h"

static uint16_t ticks = 0;
uint16_t segtab[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
uint16_t Val = 0x0000;
uint16_t strobereg;

void main()
{
	SemaphoreHandle_t LEDsem;
    BaseType_t xReturnedLED;
	TaskHandle_t xHandleLED = NULL;
	
	//create LED semaphore
	LEDsem = xSemaphoreCreateBinary();
	
	//Create LED task
	xReturnedLED = xTaskCreate(ledStrobe, "LED", configMINIMAL_STACK_SIZE, NULL, 1, &xHandleLED);
	
	vTaskStartScheduler();
}

void ledStrobe(void * pvParameters)
{
	while(1)
	{	
		wait(LEDsem,1000);
		if((PINE&0x01)==0)
			Val--;
		else if((PINE&0x02)==0)
			Val++;
		else
			Val=Val;
		
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
		signal(LEDsem);
	}
}
