#ifndef __USART_H
#define __USART_H
#include <stm32f10x.h>
#include "stdio.h"	 
	  	
extern u8 USART_RX_BUF[256];    //���ջ���,���256���ֽ� 
extern u8 USART_RX_STA;         //�������״̬��־	
extern u8 USART_RX_CNT;         //�������ݼ���

//����봮���жϽ��գ��벻Ҫע�����º궨��
//#define EN_USART1_RX //ʹ�ܴ���1����
//void uart_init(u32 pclk2,u32 bound);

#endif	   
















