
#include "stm32f10x_usart.h"
#include "misc.h"


char uart_rx_log_buffer[100] = {0};
int uart_rx_log_index = 0;

//RCCʱ������
static void RCC_cfg()
{
	//��GPIOʱ�ӣ����ù��ܣ�����1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
}

//IO������
static void GPIO_cfg()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//PA9��ΪUS1��TX�ˣ��򿪸��ã�����������
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_InitStructure);

	//PA10��ΪUS1��RX�ˣ������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

//���ڳ�ʼ��
static void USART_cfg()
{
	USART_InitTypeDef USART_InitStructure;
	//���ṹ������Ϊȱʡ״̬
	USART_StructInit(&USART_InitStructure);
	//����������Ϊ115200
	USART_InitStructure.USART_BaudRate = 115200;
	//һ֡���ݵĿ������Ϊ8bits
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//��֡��β����1��ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//��żʧ��ģʽ������żУ��
	USART_InitStructure.USART_Parity = USART_Parity_No;
	//����/����ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Ӳ��������ʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//���ô���1
	USART_Init(USART1, &USART_InitStructure);
	//�򿪴���1���ж���Ӧ����
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	//�򿪴���1
	USART_Cmd(USART1, ENABLE);
}

//�����ж�
static void NVIC_cfg()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                                               //ѡ���жϷ���2
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                              //ѡ�񴮿�1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                                 //��ռʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                                            //��Ӧʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                                 //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
}



void uart_send_data(char *data);

void process_uart_command(char *cmd) {
	
	uart_send_data(cmd);
	//uart_send_data("sending ping to 192.168.0.1\n");

		

}

void USART1_IRQHandler(void)
{
	FlagStatus RX_status;

	
	//ȷ���Ƿ���յ�����
	RX_status = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
	//���յ�����
	if(RX_status == SET)
	{
		uart_rx_log_buffer[uart_rx_log_index++] = USART_ReceiveData(USART1);
		if (uart_rx_log_buffer[uart_rx_log_index - 1] == '\r') {
			//uart_send_data(uart_rx_log_buffer);
			process_uart_command(uart_rx_log_buffer);
			memset(uart_rx_log_buffer, 0, sizeof(uart_rx_log_buffer));
			uart_rx_log_index = 0;

		}
	}
	

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{   
		
	}

}


void uart_init()
{
	RCC_cfg();
	GPIO_cfg();
	NVIC_cfg();
	USART_cfg();

	USART_ClearFlag(USART1,USART_FLAG_TC);
}

void uart_send_data(char *data)
{
	
	int len = strlen(data);
	int i = 0;

	
	for(i = 0; i < len; i++) {
		USART_SendData(USART1,data[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	}

}
