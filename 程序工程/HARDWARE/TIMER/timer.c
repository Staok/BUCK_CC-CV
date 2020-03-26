#include "timer.h"
#include "wdg.h"
#include "beep.h"
#include "SelfChecking.h"
#include "SYS_INF_CTRL.h"
#include "MENU.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

struct TIM3_IT_FLAGS Timer_IT_flags;


#define TIM3_IT_PSC (14400-1)
#define TIM3_1MS_IT_ARR (5-1)
#define TIM3_10MS_IT_ARR (50-1)// 72Mhz/14400=0.0002s  0.0002*50=0.01s
#define TIM3_100MS_IT_ARR (500-1)

#define ARR TIM3_10MS_IT_ARR //�������趨�ж϶�ʱʱ��
#define PSC TIM3_IT_PSC

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = ARR; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =PSC; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	Timer_IT_flags._10msec_flag = 0;
	Timer_IT_flags._100msec_flag = 0;
	Timer_IT_flags._1sec_flag = 0;
	Timer_IT_flags._10msec = 0;
	Timer_IT_flags._100msec = 0;
	Timer_IT_flags._1sec = 0;
    Timer_IT_flags._300msec_flag = 0;
    Timer_IT_flags._1min_flag = 0;
    Timer_IT_flags._1min = 0;
    _OutofTime_Running_flag = 0;

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
}


//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2!���ڷ��������У���ʱûɶ��������
void TIM2_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 5-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =14400-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
    
    TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx
}

u8 _OutofTime_Running_flag;
u8 run_once = 1;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		/*********
		��ʱ����ƵSTART
		*********/
		Timer_IT_flags._10msec_flag = 1;
		if(++Timer_IT_flags._10msec >= 10)
		{
			Timer_IT_flags._10msec = 0;
			Timer_IT_flags._100msec_flag = 1;
			Timer_IT_flags._100msec++;
			
		}
		
		if(Timer_IT_flags._100msec % 3 == 0)
		{
			Timer_IT_flags._300msec_flag = 1;
            IWDG_Feed();//��ι����
		}
		
		if(Timer_IT_flags._100msec >= 10)
		{
			Timer_IT_flags._100msec = 0;
			Timer_IT_flags._1sec_flag = 1;
			Timer_IT_flags._1sec++;
		}
		
		if(Timer_IT_flags._1sec >= 60)
		{
            Timer_IT_flags._1sec = 0;
			Timer_IT_flags._1min_flag = 1;
			Timer_IT_flags._1min++;
			
			
			if((Timer_IT_flags._1min > SetMaxRuningMins)){
			Timer_IT_flags._1min = 0;_OutofTime_Running_flag = 1;
			CTRL_DCDC_ON_OFF = DCDC_OFF;}//����ʱ�䣬ǿ�ƹر�
		}
		/*********
		��ʱ����ƵEND
		*********/
	}
}


//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    	/*********
		��������ӦSTART
		*********/
		if(is_buzzer_once)  //�л�һ��
		{
			if(Timer_IT_flags._100msec % 3 != 0)
			{	buzzer = ~buzzer;}else{	buzzer = 0;is_buzzer_once = 0;}
		}
		
		if(is_buzzer_bibi)  //1���Ъ�л�
		{
			run_once = 1;
			if(Timer_IT_flags._1sec % 2 == 0)
			{
				buzzer = ~buzzer;
			}else{buzzer = 0;}  //0Ϊ�ر�
		}else{
			if(run_once){	buzzer = 0;run_once = 0;}
		}
		/*********
		��������ӦEND
		*********/
    }
}












