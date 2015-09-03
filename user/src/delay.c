#include <stm32f10x.h>
#include "delay.h"


#define SYSCLK 72
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

/*
 * SysTick is the base address of systick
 * max delay is 1.8s
 *
 */
void delay_init()
{
	/* using external source, HCLK/8*/
	SysTick->CTRL &= (~BIT(2));
	/* disable interrupt of systick */
	SysTick->CTRL &= (~BIT(1));
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}	

/* SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
 * nms<=0xffffff*8*1000/SYSCLK
 * SYSCLK��λΪHz,nms��λΪms
 * ��72M������,nms<=1864 
 */
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do {
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
}   
/* ��ʱnus
 * nusΪҪ��ʱ��us��.	
 */
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}





