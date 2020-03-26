#ifndef __SYS_INF_CTRL_H__
#define __SYS_INF_CTRL_H__
#include "sys.h"

//�����¶ȣ�һ���豸(DS18B20����)��һ���ڲ�AD�¶ȶ��ģ�
//��ʾ��ʱ���ȡ��ÿ1sˢ�£�ˢ����ʾʱ��С����ÿ300ms��ϣ����ʱֻ������
extern short Temperature_DEV;//�����ŵ�����,С�������ڶ�ȡ����ʾʱ������ȡ
extern short Temperature_MCU;
extern u16 MaxRuningMins;
extern u16 SetMaxRuningMins;
//����������ʱ�䣬���趨���ʱ�䣬�����趨ʱ��Ͷ�ʱ����¼������ʱ��

//DCDC����
#define DCDC_ON 1
#define DCDC_OFF 0//д0�ر�DCDC
#define CTRL_DCDC_ON_OFF PAout(8)

//DCDC���ڵ�Դ���ǳ��ģʽ
#define DCDC_Mode_DC 1
#define DCDC_Mode_Chr 0
extern u8 CTRL_DCDC_Mode;

//DCDC���ں�ѹ���Ǻ���״̬
#define DCDC_CC 0
#define DCDC_CV 1//д1Ϊѡ���ѹ��������ѹ
#define CTRL_DCDC_CC_CV PAout(1)

#define Adc_Average_times 5


//���׶�״̬
extern u8 SYS_Chr_State;
#define Chr_State_sup 1//�����磬ĳ�ڵ�ص�ѹ����������С��ѹ����ʱ������ƽ��ģ��
#define Chr_State_CC 2//������磬ÿ�ڵ�ص�ѹ������(��û������)�����ҵ�����ѹС�������ѹ
#define Chr_State_CV 3//��ѹ��磬��������ѹ�ﵽ�����ѹ
#define Chr_State_trickle 4//�����磬������󣬵ȴ������ӵ�ʮ�����ӣ�������ѹ���䵽�������ѹ��ֵ���ٴӺ�����ʼ���
#define Chr_State_Finish 5//�����־�������ز�ȡ������ڵ�ѹ�½������������ֵʱ�ٴγ��

extern float LowVol_th;//��������ֵ
extern float FinishCur;//��ѹ����£�������ʱ��������ֵ
extern float TriVol_th;//�����磬�ٳ����ֵ

__packed struct DCDC_INF
{
	float Cur,SetCur;//ʵ�ʶ�ȡֵ���趨ֵ
	float Vol,SetVol,ChrVol;
    float Cur_Limit,Vol_Limit;//ѹ������
    float Cur_MinLimit,Vol_MinLimit;//ѹ������
	float AD5Key_Value;
	float BATCellVol_1,BATCellVol_2,BATCellVol_3,BATCellVol_4;//ÿ�ڵ�صĵ�ѹ
	u8 BATCellDisChr_1,BATCellDisChr_2,BATCellDisChr_3,BATCellDisChr_4;//ÿ�ڵ���Ƿ����ŵ磬1Ϊ��
	u8 NumofBatCells;
};

extern struct DCDC_INF DCDC_Val;
	
void Sys_ParameterInit(void);//������ʼ��
void Sys_AutoRun_Chr_Sequence(void);//������������������
void Sys_AutoRun_DC_Output(void);//���������������������ݺ�����ѹ��ѹ����Ϣ���

void Sys_ReadinALLInf(void);//һ���Զ���������Ϣ


#define _373_CS PBout(1)
#define _373_EN 1
#define _373_disEN 0

#define Balance_1 PBout(8)//��
#define Balance_2 PBout(9)
#define Balance_3 PBout(10)
#define Balance_4 PBout(11)//��
#define Balance_ON 0
#define Balance_OFF 1

//ͨ��ѡ��
#define ch_C0 0
#define ch_C1 1
#define ch_C2 2
#define ch_C3 3
#define ch_C4 4
#define ch_Cur 6
#define ch_Vol 7

void SetDAC_Value(float DAC_Value);//�趨DAC���ֵ
void Sel_CD4051_Ch(u8 ch);
void Balance_ctrl(void);
void decideBatCells(void);
void initMCP4822(void);




#endif

