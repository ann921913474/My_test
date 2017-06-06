#include "LPC11xx.h"                    // Device header

#define LED1_ON()  (LPC_GPIO2->DATA&=~(1<<5))
#define LED1_OFF() (LPC_GPIO2->DATA|=(1<<5))

#define KEY1_DOWN() ((LPC_GPIO3->DATA&(3<<0))&0x01==1)

#define KEY2_DOWN2() (LPC_GPIO3->DATA>>1&0x01)
                      
void LEDInit()
{	
  LPC_GPIO2->DIR|=(1<<5);
  LPC_GPIO2->DATA|=(1<<5);
}

void delay()
{
 int i,j;
 for(i=0;i<4000;i++)
 for(j=0;j<40;j++);	
}

int main()
{
	 
    LEDInit();
	 
	
	while(1)
	{
			
    if(KEY1_DOWN())
		{
			delay();
			if(KEY1_DOWN())
			LED1_ON();
		}
		
		if(KEY2_DOWN2()==0)
		{
			delay();
			if(KEY2_DOWN2()==0)
			LED1_OFF();
		}
	
			
		}
		
		

}



