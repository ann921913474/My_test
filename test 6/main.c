#include "LPC11xx.h"                    // Device header
uint8_t table[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uint8_t counter = 0;

void delay()
{
	uint16_t i=4000;
	while(i--);

}



void PIOINT3_IRQHandler()
{
	if((LPC_GPIO3->MIS&(1<<3)) == (1<<3))
	{
		delay();
	
	  if((LPC_GPIO3->MIS&(1<<3)) == (1<<3))
		delay();
		
		counter++;
		LPC_GPIO3->IC |= (1<<3);
	}
		
}




void Display(uint32_t value)
{
	
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value/1000];
	LPC_GPIO2->DATA&=~(1<<8);
	delay();
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000/100];
	LPC_GPIO2->DATA&=~(1<<9);
	delay();
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000%100/10];
	LPC_GPIO2->DATA&=~(1<<10);
	delay();
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000%1000%10];
	LPC_GPIO2->DATA&=~(1<<11);
	delay();	
	LPC_GPIO2->DATA =0xFFF;
	
}	


int main(void)
{
	
	LPC_GPIO2->DIR |=0xFFF;
	LPC_GPIO3->DIR &=~(1<<3);
	LPC_GPIO3->IE  |=(1<<3) ;
	LPC_GPIO3->IS  &=~(1<<3);
	LPC_GPIO3->IEV &=~(1<<3);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	while(1)
	{
		Display(counter);
	}
	
}
