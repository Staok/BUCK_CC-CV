#ifndef __MENU_H__
#define __MENU_H__

#include "sys.h"

extern short Temperature_DEV;//�����ŵ�����
extern short Temperature_MCU;


//ADKey״̬
extern u8 KeyPos;//������־��
#define Key_Null 0
#define Key_Down 1
#define Key_Left 2
#define Key_Up 3
#define Key_Center 4
#define Key_Right 5

//extern u8 is_Key_Down;//�Ƿ��Ѿ����£����ڵ�����Ч����
//extern u8 is_Key_Left;
//extern u8 is_Key_Up;
//extern u8 is_Key_Center;
//extern u8 is_Key_Right;

extern u8 is_Coarse;//�Ƿ�ֵ�����ѹ����������0.01��Ϊ0.1��ʱ����1��Ϊ10

extern u8 Key_CenterCount;//�����������ɿ���0�������м�3S�л�ģʽ
extern u8 is_Key_Center_LongPrass;//������־���ɿ���0
extern u8 Key_UseUp;

void clearAllKeyInf(void);


void DrawPageHead(u8 *str);//ҳ������ʾ����Դģʽ�����ߡ�ƽ����ģʽ��
void DrawPowerModePage(void);//������ѹԴ�Ļ�������
void DrawChargeModePage(void);//ƽ���Ļ������棨���ڻ�����Ϣ�������������ƽ��ģ����Ϣ��ʾ��
void DrawCommonInf(void);//����ģʽ���е���Ϣ����

void Draw_Refresh_NumofBatCells(void);
void Draw_Refresh_BalanceChrInf(void);//����ƽ��ģ����Ϣ��ʾ
void Draw_Refresh_ONOFF(void);//���¡����С���ͣ����״̬��Ϣ
void Draw_Refresh_PowSignal(void);//������ʱģ���̵�������ͣ��ʱ��ʾ���
void Draw_Refresh_CCCV(void);//���¡�����/��ѹ��...����Ϣ
void Draw_Refresh_CurVol(void);//����ʵʱ��ȡ�ĵ�����ѹ��Ϣ
void Draw_Refresh_Tem(void);//����ʵʱ�¶ȶ�ȡ��Ϣ
void Draw_Refresh_RuningTime(void);//��������ʱ����Ϣ
void Draw_Refresh_All(void);//���µ�ǰ��Ļ�����б�����Ϣ

//���������Ӧ
void Opera_DC_Menu(void);
void Opera_Chr_Menu(void);
void MENU_Refrash_Pos(void);

//ƽ���ģʽ�˵�����
extern u8 Menu_Chr_Pos;
#define Menu_Chr_DCDC_ONOFF 1
#define Menu_Chr_SetCur 2
#define Menu_Chr_SetTime 3

#define Menu_Chr_MaxItem 3

//��Դģʽ�˵�����
extern u8 Menu_DC_Pos;
#define Menu_DC_DCDC_ONOFF 1
#define Menu_DC_SetCur 2//Set�ĸ����ͽ����ĸ��Ŀ���
#define Menu_DC_SetVol 3
#define Menu_DC_SetTime 4

#define Menu_DC_MaxItem 4


//char *myitoa(int value, char *string, int radix);
//int myatoi(const char *str);

#endif

