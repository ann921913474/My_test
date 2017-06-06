#include "LPC11xx.h"                    // Device header

int main(void)
{
	LPC_SYSCON ->SYSAHBCLKCTRL|=(1<<16);
	LPC_IOCON->PIO2_3&=0XF8;
	LPC_GPIO2->DIR|=(1<<3);
  LPC_GPIO2->DATA&=~(1<<3);
}
	
