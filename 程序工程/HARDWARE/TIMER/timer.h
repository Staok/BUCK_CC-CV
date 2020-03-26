#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
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

__packed struct TIM3_IT_FLAGS
{
	u8 _10msec_flag;
	u16 _10msec;
	u8 _100msec_flag;
	u16 _100msec;
	u8 _300msec_flag;
	u8 _1sec_flag;
	u16 _1sec;
	u8 _1min_flag;
	u16 _1min;
};
extern struct TIM3_IT_FLAGS Timer_IT_flags;
extern u8 _OutofTime_Running_flag;
extern u8 run_once;

void TIM3_Int_Init(void);
void TIM2_Int_Init(void);
 
#endif
