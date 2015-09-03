
#include "pwm.h"
#include "delay.h"




/* refer to http://blog.sina.com.cn/s/blog_49cb42490100s6uh.html */
//duty factor, 0-100
int dutyfactor = 50;
int tim_counter = 1895;//7200


static void RCC_cfg()
{
	//����TIM3��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//����GPIOB��ʱ�Ӻ͸��ù���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
}

static void GPIO_cfg()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//����ӳ�䣬��TIM3_CH2ӳ�䵽PB5
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	//ѡ������5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	//���Ƶ�����50MHz                                                        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//�����������                                              
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

static void TIMER_cfg()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//���½�Timer����Ϊȱʡֵ
	TIM_DeInit(TIM3);
	//�����ڲ�ʱ�Ӹ�TIM3�ṩʱ��Դ
	TIM_InternalClockConfig(TIM3);
	//Ԥ��Ƶϵ��Ϊ0����������Ԥ��Ƶ����ʱTIMER��Ƶ��Ϊ72MHz
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//���ü��������С��ÿ��7200�����Ͳ���һ�������¼�����PWM�����Ƶ��Ϊ10kHz
	TIM_TimeBaseStructure.TIM_Period = tim_counter - 1;
	//������Ӧ�õ�TIM3��
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
}

static void PWM_cfg()
{
	TIM_OCInitTypeDef TimOCInitStructure;
	//����ȱʡֵ
	TIM_OCStructInit(&TimOCInitStructure);
	//PWMģʽ1���
	TimOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//����ռ�ձȣ�ռ�ձ�=(CCRx/ARR)*100%��(TIM_Pulse/TIM_Period)*100%
	TimOCInitStructure.TIM_Pulse = dutyfactor * tim_counter / 100;
	//TIM����Ƚϼ��Ը�
	TimOCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//ʹ�����״̬
	TimOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM3��CH2���
	TIM_OC2Init(TIM3, &TimOCInitStructure);
	//����TIM3��PWM���Ϊʹ��
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
}


void init_pwm()
{
	RCC_cfg();
	GPIO_cfg();
	TIMER_cfg();
	PWM_cfg();
}

void pwm_set_enable(int enable) {
	if (enable)
		TIM_Cmd(TIM3, ENABLE);
	else
		TIM_Cmd(TIM3, DISABLE);

}








