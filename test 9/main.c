#include "LPC11xx.h"                    // Device header
#include<stdio.h>
#include<string.h>
#define UART_BPS 9600//串口通信波特率
char GcRcvBuf[20];//AD采集到的数据


/**********************************
软件延时
***********************************/
void Delay(uint32_t ulTime)
{
 uint32_t i;
 i = 0;
 while(ulTime--)
 {
  for(i=0;i<5000;i++);
	 
 }
 
	} 
/**********************************
	LED初始化
	*********************************/
	void LED_Init(void)
	{
   
		
  LPC_GPIO2->DIR|=(1<<0);
	LPC_GPIO2->DIR|=(1<<1);
	LPC_GPIO2->DIR|=(1<<2);
	LPC_GPIO2->DIR|=(1<<3);
	LPC_GPIO2->DIR|=(1<<4);
	LPC_GPIO2->DIR|=(1<<5);
	LPC_GPIO2->DIR|=(1<<6);
	LPC_GPIO2->DIR|=(1<<7);

  LPC_GPIO2->DATA|=(1<<0);
  LPC_GPIO2->DATA|=(1<<1);
  LPC_GPIO2->DATA|=(1<<2);
  LPC_GPIO2->DATA|=(1<<3);
  LPC_GPIO2->DATA|=(1<<4);
  LPC_GPIO2->DATA|=(1<<5);
  LPC_GPIO2->DATA|=(1<<6);

	
	}
	
	/*********************************
	ADC初始化
	**********************************/
	void ADC_Init(void)
	{
	  LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16);
		LPC_IOCON->R_PIO0_11&=~0xBF;//配置PIO1_11为模拟输入模式
		LPC_IOCON->R_PIO0_11|=0x02;//PIO0_11模拟输入通道0
		LPC_SYSCON->PDRUNCFG&=~(0x01<<4);//ADC模块上电
		LPC_SYSCON->SYSAHBCLKCTRL|=(0x01<<13);//使能ADC模块时钟
		LPC_ADC->CR=(0x01<<0)|  //SEL=1，选择ADC7
		            ((SystemCoreClock/1000000-1)<<8)|//转换时钟1MHz
		            (0<<16)|//软件控制转换操作
		            (0<<17)|//使用11clocks转换
		            (0<<24)|//ADC转换停止
		            (0<<27);//直接启动ADC转换，此位无效
	}
	/**********************************
 串口初始化，设置为8位数据位，1位停止位，无奇偶校验，波特率为115200
	***********************************/
	void UART_Init(void) 
	{
	 uint16_t usFdiv;
	 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16); //打开IOCON配置时钟
	 LPC_IOCON->PIO1_6&=~0x07;
	 LPC_IOCON->PIO1_6|=(1<<0);//配置p1.6为RXD
	 LPC_IOCON->PIO1_7&=~0x07;
	 LPC_IOCON->PIO1_7|=(1<<0);//配置P1.7为TXD
	 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<12);//打开UART功能部件时钟
	 LPC_SYSCON->UARTCLKDIV=0x01;//UART时钟分频
	 LPC_UART->LCR=0x83;//允许设置波特率
	 usFdiv=(SystemCoreClock/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;//设置波特率
	 LPC_UART->DLM = usFdiv/256;
	 LPC_UART->DLL = usFdiv % 256;
	 LPC_UART->LCR = 0x03;//锁定波特率
	 LPC_UART->FCR = 0x07;
	}
	/************************************
	向串口发送字节数据，并等待数据发送完成，使用查询方式
	ucDat:要发送的数据
	*************************************/
	void UART_SendByte(uint8_t ucDat)
	{
	 LPC_UART->THR=ucDat;//写入数据
	 while((LPC_UART->LSR&0x40)==0);//等待数据发送完毕		
	
	}
	/*************************************
	向串口发送字符串
	puiStr:要发送的字符串指针
	**************************************/
	void UART_SendStr(char * pucStr)
	{
	 
		while(1){
		      if(*pucStr=='\0')break;//遇到结束符，退出
			    UART_SendByte(*pucStr++);
		
		}
	}
	/**************************************
	主函数
	打开串口调试助手，运行程序；
	串口调试助手显示下位机返回的采样数据
	***************************************/
	int main(void)
	{
	 uint32_t i;
	 int OldV=0;
	 int j;
	 uint32_t t=0;
	 uint32_t ulADCData;
	 uint32_t ulADCBuf;
	 UART_Init();//串口初始化
		
		
   ADC_Init();//ADC模式初始化
	 LED_Init();//led灯初始化
		
	 while(1)
	    {
				t++;
				t%=1;
				
				
				ulADCData=0;
				for(i=0;i<10;i++)
				{
					LPC_ADC->CR|=(1<<24);//立即转换
					while((LPC_ADC->DR[0]&0x80000000)==0);//读取DR0的Done
					LPC_ADC->CR|=(1<<24);//第一次转换结果丢弃
					while((LPC_ADC->DR[0]&0x80000000)==0);//读取DR0的Done
					ulADCBuf=LPC_ADC->DR[0];//读取结果寄存器
					ulADCBuf=(ulADCBuf>>6)&0x3ff;//数据从第6位开始储存，占据10位
					ulADCData+=ulADCBuf;//数据累加
				}
					
				
    			ulADCData=ulADCData/10;//采样10次进行滤波处理
				ulADCData=(ulADCData*3300)/1024;  //数字量转换成模拟量
				sprintf(GcRcvBuf,"VIN0=%4dmV\r\n",ulADCData);  //将数据发送到串口进行显示
				UART_SendStr(GcRcvBuf);//将数据发送到串口显示
				
				if(ulADCData>OldV+5)
				{
				if(t==0)
				{
				 
					LPC_GPIO2->DATA&=~(1<<j);
					j++;
					if(j>8)j=8;
					
				}OldV=ulADCData;
	
		 	}
				else if(t==0&&ulADCData<OldV-5)
				{	
				  LPC_GPIO2->DATA|=(1<<j-1);
					j--;
				  if(j<=0)j=0;
					
					
				}
				OldV=ulADCData;
     	
			}
				
			
				
	 
	 }
	 
	 
	 
	 
	 
	 
	 
	 