#ifndef __SELFCHECKING_H
#define __SELFCHECKING_H	 
#include "sys.h"

extern short Temperature_DEV;//�������¶��з���
extern short Temperature_MCU;

/*���ϴ���
0:->Ŀǰ�޹��ϣ�
1:->�¶Ȳ�����ʼ��ʧ�ܣ�
2:->DCDCоƬû�������رգ�
3:->DCDCоƬû�������򿪣�
4:->����¶ȹ��߻���MCU�¶ȹ���
*/
#define Error_ds18b20_isn_init 1
#define Error_DCDC_isn_shut 2
#define Error__reserve 3
#define Error_Tem_Over_Scope 4
#define Error_DCDC_Output_isn_Normal 5

extern u8 Error_code;
extern u16 ShutSecondNow;

//�Լ�DCDC�Ƿ������رգ������ڳ���ر�DCDCоƬ֮����Լ�
//u8 IS_DCDC_OFF(void);
//�Լ��¶��Ƿ���������Χ
u8 is_Tem_Over_Scope(void);
//�Լ쵱DCDCоƬ����ʱ�����ж����ڴ��ں������Ǻ�ѹģʽ���ٸ����ж�����Ƿ�����
u8 is_DCDC_Output_Normal(void);

#define Error_check_num 2//ѭ������������Ϻ�������Ŀ

void dealWith_Output_isn_Normal(void);
//void dealWith_DCDC_isn_Shut(void);
void dealWith_ds18b20_isn_init(void);
void dealWith_Tem_Over_Scope(void);

		 				    
#endif

