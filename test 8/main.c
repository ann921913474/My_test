#include "LPC11xx.h"                    // Device header
#define UART_BPS 9600 //����ͨ�Ų�����
volatile uint8_t GucRcvNew; //���ڽ��������ݵı�־
uint8_t GucRcvBuf[10]; //���ڽ������ݻ�����
uint32_t GulNum;//���ڽ������ݵĸ���
/***************************************
���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ��λ
����FIFO����Ϊ8B
****************************************/
void UART_Init(void)
{
 uint16_t usFdiv;
 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16);   //ʹ��IOCONʱ��
 LPC_IOCON->PIO1_6|=0x01;       //��P1.6��P1.7����ΪRXD��TXD
 LPC_IOCON->PIO1_7|=0x01;
 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<12);//��UART���ܲ���ʱ��
 LPC_SYSCON->UARTCLKDIV   =0x01; //UARTsʱ�ӷ�ƵΪ1	
 LPC_UART->LCR =0x83; //�������ñ����� 
 usFdiv = (SystemCoreClock/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;  //���ò�����
 LPC_UART->DLM = usFdiv/256;
 LPC_UART->DLL = usFdiv %256;
 LPC_UART->LCR =0x03; //����������
 LPC_UART->FCR =0x07; //ʹ��FIFO������8���ֽڴ�����
 NVIC_EnableIRQ(UART_IRQn); //ʹ�ܽ����ж�
 NVIC_SetPriority(UART_IRQn,1);//�����ж����ȼ�Ϊ1
 LPC_UART->IER = 0x01;//ʹ�ܽ����ж�
}



/***************************************
LED�Ƶĳ�ʼ��
****************************************/
void oled_Init(void)
{

  LPC_GPIO2->DIR|=(1<<4);
//	LPC_GPIO2->DATA&=~(1<<4);
    LPC_GPIO2->DATA|=GucRcvBuf[1];
}	
 


/****************************************
����һ�ֽ����ݲ��ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
ucDat:Ҫ���͵�����
****************************************/
void UART_SendByte(uint8_t ucDat)
{

  LPC_UART->THR = ucDat; //д������
	while((LPC_UART->LSR&0x40) ==0);//�ȴ����ݷ������
}
/****************************************
�򴮿ڷ����ַ���
putStr:Ҫ���͵��ַ���ָ��,ulNum:Ҫ���͵����ݸ���
*****************************************/
void UART_SendStr(uint8_t const * pucStr,uint32_t ulNum)
{
   
	uint32_t i;
	for(i=0;i<ulNum;i++) //д��ָ���ֽ�����
	{
	
				UART_SendByte(*pucStr++);

	}
}
/****************************************
UART�жϷ�����
*****************************************/
void  UART_IRQHandler(void)
{
 GulNum=0;
 while((LPC_UART->IIR&0x01)==0)   //�ж��Ƿ����жϹ���
 {
     switch(LPC_UART->IIR&0x0E)  //�ж��жϱ�־
		 {
		   case 0x04:   //���������ж�
			    GucRcvNew = 1;   //�ý��������ݱ�־
			    for(GulNum = 0;GulNum<8;GulNum++) //��������8���ֽ�
				 {
						GucRcvBuf[GulNum] = LPC_UART->RBR;
				 }
				 break;
			case 0x0C: //�ַ���ʱ�ж�
				 GucRcvNew = 1;
			   while((LPC_UART->LSR&0x01) == 0x01) //�ж������Ƿ�������
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
	���������ѴӴ��ڽ��յ������ݷ��ͻ�ȥ
	*************************************/
	int main(void)
	{
	 oled_Init();
	 UART_Init(); //���ڳ�ʼ��
	 while(1)
	 {
	     if(GucRcvNew==1)   //�ж��Ƿ���������
			 {
			   GucRcvNew = 0; //�����־
				 UART_SendStr(GucRcvBuf,GulNum);//�򴮿ڷ�������
				 
				 if(GucRcvBuf[0]==0x49)
         LPC_GPIO2->DATA=GucRcvBuf[0];
				 else LPC_GPIO2->DATA|=(1<<4);
				 
			 }
	 
	 
	 
	 }		 
	
	
	
	
	}
	



