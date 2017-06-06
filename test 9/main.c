#include "LPC11xx.h"                    // Device header
#include<stdio.h>
#include<string.h>
#define UART_BPS 9600//����ͨ�Ų�����
char GcRcvBuf[20];//AD�ɼ���������


/**********************************
�����ʱ
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
	LED��ʼ��
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
	ADC��ʼ��
	**********************************/
	void ADC_Init(void)
	{
	  LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16);
		LPC_IOCON->R_PIO0_11&=~0xBF;//����PIO1_11Ϊģ������ģʽ
		LPC_IOCON->R_PIO0_11|=0x02;//PIO0_11ģ������ͨ��0
		LPC_SYSCON->PDRUNCFG&=~(0x01<<4);//ADCģ���ϵ�
		LPC_SYSCON->SYSAHBCLKCTRL|=(0x01<<13);//ʹ��ADCģ��ʱ��
		LPC_ADC->CR=(0x01<<0)|  //SEL=1��ѡ��ADC7
		            ((SystemCoreClock/1000000-1)<<8)|//ת��ʱ��1MHz
		            (0<<16)|//�������ת������
		            (0<<17)|//ʹ��11clocksת��
		            (0<<24)|//ADCת��ֹͣ
		            (0<<27);//ֱ������ADCת������λ��Ч
	}
	/**********************************
 ���ڳ�ʼ��������Ϊ8λ����λ��1λֹͣλ������żУ�飬������Ϊ115200
	***********************************/
	void UART_Init(void) 
	{
	 uint16_t usFdiv;
	 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<16); //��IOCON����ʱ��
	 LPC_IOCON->PIO1_6&=~0x07;
	 LPC_IOCON->PIO1_6|=(1<<0);//����p1.6ΪRXD
	 LPC_IOCON->PIO1_7&=~0x07;
	 LPC_IOCON->PIO1_7|=(1<<0);//����P1.7ΪTXD
	 LPC_SYSCON->SYSAHBCLKCTRL|=(1<<12);//��UART���ܲ���ʱ��
	 LPC_SYSCON->UARTCLKDIV=0x01;//UARTʱ�ӷ�Ƶ
	 LPC_UART->LCR=0x83;//�������ò�����
	 usFdiv=(SystemCoreClock/LPC_SYSCON->UARTCLKDIV/16)/UART_BPS;//���ò�����
	 LPC_UART->DLM = usFdiv/256;
	 LPC_UART->DLL = usFdiv % 256;
	 LPC_UART->LCR = 0x03;//����������
	 LPC_UART->FCR = 0x07;
	}
	/************************************
	�򴮿ڷ����ֽ����ݣ����ȴ����ݷ�����ɣ�ʹ�ò�ѯ��ʽ
	ucDat:Ҫ���͵�����
	*************************************/
	void UART_SendByte(uint8_t ucDat)
	{
	 LPC_UART->THR=ucDat;//д������
	 while((LPC_UART->LSR&0x40)==0);//�ȴ����ݷ������		
	
	}
	/*************************************
	�򴮿ڷ����ַ���
	puiStr:Ҫ���͵��ַ���ָ��
	**************************************/
	void UART_SendStr(char * pucStr)
	{
	 
		while(1){
		      if(*pucStr=='\0')break;//�������������˳�
			    UART_SendByte(*pucStr++);
		
		}
	}
	/**************************************
	������
	�򿪴��ڵ������֣����г���
	���ڵ���������ʾ��λ�����صĲ�������
	***************************************/
	int main(void)
	{
	 uint32_t i;
	 int OldV=0;
	 int j;
	 uint32_t t=0;
	 uint32_t ulADCData;
	 uint32_t ulADCBuf;
	 UART_Init();//���ڳ�ʼ��
		
		
   ADC_Init();//ADCģʽ��ʼ��
	 LED_Init();//led�Ƴ�ʼ��
		
	 while(1)
	    {
				t++;
				t%=1;
				
				
				ulADCData=0;
				for(i=0;i<10;i++)
				{
					LPC_ADC->CR|=(1<<24);//����ת��
					while((LPC_ADC->DR[0]&0x80000000)==0);//��ȡDR0��Done
					LPC_ADC->CR|=(1<<24);//��һ��ת���������
					while((LPC_ADC->DR[0]&0x80000000)==0);//��ȡDR0��Done
					ulADCBuf=LPC_ADC->DR[0];//��ȡ����Ĵ���
					ulADCBuf=(ulADCBuf>>6)&0x3ff;//���ݴӵ�6λ��ʼ���棬ռ��10λ
					ulADCData+=ulADCBuf;//�����ۼ�
				}
					
				
    			ulADCData=ulADCData/10;//����10�ν����˲�����
				ulADCData=(ulADCData*3300)/1024;  //������ת����ģ����
				sprintf(GcRcvBuf,"VIN0=%4dmV\r\n",ulADCData);  //�����ݷ��͵����ڽ�����ʾ
				UART_SendStr(GcRcvBuf);//�����ݷ��͵�������ʾ
				
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
	 
	 
	 
	 
	 
	 
	 
	 