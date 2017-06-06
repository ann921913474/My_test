#include"LPC11xx.h"
uint8_t table[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
//void Delay();
void GpioInit(void)
{
	LPC_GPIO2->DIR |=0xFFF;
	


}
void Delay(int x)
{
	int z,y;
	for(y=x;y>=0;y--)
	    for(z=500;z>=0;z--);
}
int Display(uint32_t value)
{
	
	
  LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value/1000];
	LPC_GPIO2->DATA&=~(1<<8);
	Delay(10);
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000/100];
	LPC_GPIO2->DATA&=~(1<<9);
  Delay(10);
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000%100/10];
	LPC_GPIO2->DATA&=~(1<<10);
	Delay(10);
	LPC_GPIO2->DATA =0xFFF;
	
	LPC_GPIO2->DATA &=~(0xFF);
	LPC_GPIO2->DATA |=table[value%1000%1000%10];
	LPC_GPIO2->DATA&=~(1<<11);
	Delay(10);
	LPC_GPIO2->DATA =0xFFF;
	
}
int main()
{
	
	GpioInit();
	Delay(10);
	while(1)
	{
  Display (6789);  //为什么最后两个个数字不可以为8和9？
	}
}