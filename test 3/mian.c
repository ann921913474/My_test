#include"LPC11xx.h"
void delay(void)
{
 uint16_t i=4000;
	while(i--);

}
int main(void)
{
	LPC_GPIO2->DIR=0xFFF;
	while(1)
	{
   LPC_GPIO2->DATA &=~(0xFF);
	 LPC_GPIO2->DATA |=(0x88);
	 LPC_GPIO2->DATA &=~(1<<9);
	delay();
	LPC_GPIO2->DATA =0xFFF;
		
	}
	
}