#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

extern u8 is_buzzer_once;
extern u8 is_buzzer_bibi;

#define buzzer_once is_buzzer_once = 1;  //�������л�һ��

#define Buzzer_ON 1//��Ъ�л���
#define Buzzer_OFF 0 //��Ъ�л��ر�
#define CTRL_buzzer_ON_OFF is_buzzer_bibi


#define buzzer PCout(13)	   //����1ms��ʱ�������趨�����У�ע���˳�֮ǰӦ���͵�ƽ

void BEEP_Init(void);
		 				    
#endif

