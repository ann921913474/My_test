#include "LPC11xx.h"                    // Device header
#define UART_BPS 9600 //串口通信波特率
volatile uint8_t GucRcvNew; //串口接收新数据的标志
uint8_t GucRcvBuf[10]; //串口接收数据缓冲区
uint32_t GulNum;//串口接收数据的个数
/***************************************
串口初始化，设置为8位数据位，1位停止位，无奇偶校验位
接收FIFO设置为8B
****************************************/
void UART_Init(void)
{
 uint16_t usFdiv;
 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16);   //使能IOCON时钟
 LPC_IOCON->PIO1_6|=0x01;       //将P1.6和P1.7配置为RXD和TXD
 LPC_IOCON->PIO1_7|=0x01;
 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<12);//打开UART功能部件时钟
 LPC_SYSCON->UARTCLKDIV   =0x01; //UARTs时钟分频为1	
 LPC_UART->LCR =0x83; //允许设置比特率 
 usFdiv = (SystemCoreClock/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;  //设置波特率
 LPC_UART->DLM = usFdiv/256;
 LPC_UART->DLL = usFdiv %256;
 LPC_UART->LCR =0x03; //锁定波特率
 LPC_UART->FCR =0x07; //使能FIFO，设置8个字节触发点
 NVIC_EnableIRQ(UART_IRQn); //使能接收中断
 NVIC_SetPriority(UART_IRQn,1);//设置中断优先级为1
 LPC_UART->IER = 0x01;//使能接收中断
}



/***************************************
LED灯的初始化
****************************************/
void oled_Init(void)
{

  LPC_GPIO2->DIR|=(1<<4);
//	LPC_GPIO2->DATA&=~(1<<4);
    LPC_GPIO2->DATA|=GucRcvBuf[1];
}	
 


/****************************************
发送一字节数据并等待数据发送完成，使用查询方式
ucDat:要发送的数据
****************************************/
void UART_SendByte(uint8_t ucDat)
{

  LPC_UART->THR = ucDat; //写入数据
	while((LPC_UART->LSR&0x40) ==0);//等待数据发送完毕
}
/****************************************
向串口发送字符串
putStr:要发送的字符串指针,ulNum:要发送的数据个数
*****************************************/
void UART_SendStr(uint8_t const * pucStr,uint32_t ulNum)
{
   
	uint32_t i;
	for(i=0;i<ulNum;i++) //写入指定字节数据
	{
	
				UART_SendByte(*pucStr++);

	}
}
/****************************************
UART中断服务函数
*****************************************/
void  UART_IRQHandler(void)
{
 GulNum=0;
 while((LPC_UART->IIR&0x01)==0)   //判断是否有中断挂起
 {
     switch(LPC_UART->IIR&0x0E)  //判断中断标志
		 {
		   case 0x04:   //接收数据中断
			    GucRcvNew = 1;   //置接收新数据标志
			    for(GulNum = 0;GulNum<8;GulNum++) //连续接收8个字节
				 {
						GucRcvBuf[GulNum] = LPC_UART->RBR;
				 }
				 break;
			case 0x0C: //字符超时中断
				 GucRcvNew = 1;
			   while((LPC_UART->LSR&0x01) == 0x01) //判断数据是否接收完毕
				 {
				    GucRcvBuf[GulNum]=LPC_UART->RBR;
					  GulNum++;
				 }					
        break;
       default:
        break;
       }			 
		}
  }
/**************************************
	主函数，把从串口接收到的数据发送回去
	*************************************/
	int main(void)
	{
	 oled_Init();
	 UART_Init(); //串口初始化
	 while(1)
	 {
	     if(GucRcvNew==1)   //判断是否有新数据
			 {
			   GucRcvNew = 0; //清除标志
				 UART_SendStr(GucRcvBuf,GulNum);//向串口发送数据
				 
				 if(GucRcvBuf[0]==0x49)
         LPC_GPIO2->DATA=GucRcvBuf[0];
				 else LPC_GPIO2->DATA|=(1<<4);
				 
			 }
	 
	 
	 
	 }		 
	
	
	
	
	}
	



