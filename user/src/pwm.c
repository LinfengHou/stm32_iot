

#include "stm32f10x_tim.h"
#include "delay.h"


#define NEC_HDR_MARK    4500
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
/* compensate for accuracy */
#define PWM_ON_DELAY 13
#define PWM_OFF_DELAY 15
#define BIT(x) (1<<x)


//refer to http://blog.sina.com.cn/s/blog_49cb42490100s6uh.html
//duty factor, 0-100
int dutyfactor = 50;
int tim_counter = 1895;//7200


//test code
static void GPIO_PA7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 						 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIO_WriteBit(GPIOA, GPIO_Pin_7, 1);

}



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









void output_38k(int delay)
{
	pwm_set_enable(1);
	delay_us(delay + PWM_ON_DELAY);
}
void output_38k_off(int delay)
{
	pwm_set_enable(0);
	delay_us(delay - PWM_OFF_DELAY);
}

void send_bit(int bit)
{
	//printf("send %d\n", bit);
	if (bit == 1) {
		output_38k(NEC_BIT_MARK);
		output_38k_off(NEC_ONE_SPACE);
	} else if (bit == 0) {
		output_38k(NEC_BIT_MARK);
		output_38k_off(NEC_ZERO_SPACE);
	} else {
		//printf("critical error!\n");
	}
}

void send_byte(unsigned char byte)
{
	int  i = 0;

	for (i = 7; i >= 0; i--)
		send_bit(((byte & BIT(i)) >> i));
		
}

void send_ir_data(unsigned char* data, int len)
{
	int i  = 0;
	
	output_38k(NEC_HDR_MARK);
	output_38k_off(NEC_HDR_MARK);

	for (i = 0; i < len; i++)
		send_byte(data[i]);

	send_bit(1);
	delay_us(NEC_HDR_MARK- PWM_OFF_DELAY);

}

/* 0xb2,0x4d,0x9f,0xe0,0xd8,0xaf */
unsigned int rawCodes_ac_open[] = {4397,4368,561,1578,560,537,557,1581,584,1582,584,486,558,537,584,1554,
	560,538,558,510,558,1607,557,513,585,511,556,1585,581,1583,583,486,557,1609,556,513,558,538,557,512,558,
	1607,558,1583,557,1608,558,1608,557,1580,559,1606,559,1581,556,1609,557,512,559,537,558,512,557,539,557,
	512,558,1607,558,1582,558,537,584,1555,558,1607,556,514,559,537,556,513,558,537,556,514,558,1606,559,511,
	583,513,558,1582,557,1608,558,1580,585,5154,4419,4347,584,1555,558,539,557,1581,584,1581,571,501,577,516,
	571,1568,557,541,581,487,557,1608,557,513,558,538,557,1582,583,1582,584,487,557,1608,557,511,611,486,558,
	510,558,1607,557,1609,556,1583,583,1582,583,1555,558,1607,558,1607,559,1582,557,537,585,486,558,510,558,
	540,558,509,557,1608,558,1608,557,512,559,1606,559,1581,558,537,581,490,558,510,587,511,557,511,559,1606,
	559,510,569,527,558,1582,560,1604,561,1605,561};




void send_raw(int *code, int size)
{

	int i = 0;


	for (i = 0; i < size; i++) {
		if (i%2 == 0) {
			output_38k(code[i]);
		} else {
			output_38k_off(code[i]);
			
		}
	}
	
}


