#include "SYS_INF_CTRL.h"
#include "sys_io.h"
#include "SelfChecking.h"
#include "ds18b20.h"
#include "tsensor.h"
#include "adc.h"
#include "delay.h"
#include "MENU.h"
#include "timer.h"

short Temperature_DEV;//�����ŵ�����
short Temperature_MCU;
u16 MaxRuningMins;
u16 SetMaxRuningMins;

float LowVol_th;//��������ֵ
float FinishCur;//��ѹ����£�������ʱ��������ֵ
float TriVol_th;//�����磬�ٳ����ֵ

u8 CTRL_DCDC_Mode;
u8 SYS_Chr_State;

struct DCDC_INF DCDC_Val;

//����Ĭ�ϲ����趨
void Sys_ParameterInit(void)
{
	CTRL_DCDC_ON_OFF = DCDC_OFF;
	CTRL_DCDC_CC_CV = DCDC_CV;//�������ģʽ������׼��״̬
	CTRL_DCDC_Mode = DCDC_Mode_DC;
    
	MaxRuningMins = 600;				//��������ʱ�����Ƶ����ޣ����ظ�
	SetMaxRuningMins = 120;				
    
    DCDC_Val.NumofBatCells = 0;//��ؽ���
	
    DCDC_Val.Cur_Limit = 2.0;
    DCDC_Val.Vol_Limit = 20.0;
    DCDC_Val.Cur_MinLimit = 0.50;//���������Ʋ���32mv����趨����̫С�Ͳ����ȶ�
    DCDC_Val.Vol_MinLimit = 3.00;
    
	DCDC_Val.Cur = 0;//���ռ��λ����    ->0.00~3.00A
	DCDC_Val.Vol = 0;//              ->0.00~9.99~20.00V  ��ͬ
	DCDC_Val.SetCur = DCDC_Val.Cur_Limit;
    DCDC_Val.SetVol = 12.60;//��ѹ״̬
    DCDC_Val.ChrVol = 0;//���ģʽ�趨��ѹ
	DCDC_Val.BATCellVol_1 = 0;
	DCDC_Val.BATCellVol_2 = 0;
	DCDC_Val.BATCellVol_3 = 0;
	DCDC_Val.BATCellVol_4 = 0;
    
	DCDC_Val.BATCellDisChr_1 = 0;
	DCDC_Val.BATCellDisChr_2 = 0;
	DCDC_Val.BATCellDisChr_3 = 0;
	DCDC_Val.BATCellDisChr_4 = 0;
    
    LowVol_th = 3.5;//��������ֵ
    FinishCur = 0.5;//��ѹ����£�������ʱ��������ֵ������С��DCDC_Val.Cur_MinLimit
    TriVol_th= 4.1;//�����磬�ٳ����ֵ
    
    DCDC_Val.AD5Key_Value = 100.0;
    
    SYS_Chr_State = Chr_State_CC;//Ĭ�Ϻ���״̬
    
    KeyPos = 0;
//    is_Key_Down = 0;
//    is_Key_Left = 0;
//    is_Key_Up = 0;
//    is_Key_Center = 0;
//    is_Key_Right = 0;
    is_Coarse = 0;
    Key_CenterCount = 0;
    is_Key_Center_LongPrass = 0;
    Key_UseUp = 0;
    
    Menu_Chr_Pos = 1;
    Menu_DC_Pos = 1;

	Temperature_DEV = 27;
	Temperature_MCU = 27;
}

//�����򣬲˵�ѡ��ƽ���ģʽ����������������
void Sys_AutoRun_Chr_Sequence(void)
{
    static u16 Finish_min;//�뿪���κ���ֵ����
    
    /********
    �����״̬���ж�Ӧ����ʲô���׶�START
    ********/
    //ȷ���Ƿ���в�����(�е�ص�ѹ����)���޸Ĳ���
    switch(DCDC_Val.NumofBatCells)
    {
        case 1:if(DCDC_Val.BATCellVol_1 < LowVol_th){SYS_Chr_State = Chr_State_sup;}
            break;
        case 2:if((DCDC_Val.BATCellVol_1 < LowVol_th)||(DCDC_Val.BATCellVol_2 < LowVol_th)){SYS_Chr_State = Chr_State_sup;}
            break;
        case 3:if((DCDC_Val.BATCellVol_1 < LowVol_th)||(DCDC_Val.BATCellVol_2 < LowVol_th)||(DCDC_Val.BATCellVol_3 < LowVol_th))\
            {SYS_Chr_State = Chr_State_sup;}
            break;
        case 4:if((DCDC_Val.BATCellVol_1 < LowVol_th)||(DCDC_Val.BATCellVol_2 < LowVol_th)||(DCDC_Val.BATCellVol_3 < LowVol_th)||\
            (DCDC_Val.BATCellVol_4 < LowVol_th)){SYS_Chr_State = Chr_State_sup;}
            break;
        default:break;
    }
    //���������������ҵ�ѹС�������ѹ����ѡ��������״̬
    if((SYS_Chr_State != Chr_State_sup)&&(DCDC_Val.Vol < (DCDC_Val.NumofBatCells*4.20)))
    {
        SYS_Chr_State = Chr_State_CC;
    }
    //���������������ҵ�ѹ�ﵽ�����ѹ����ѡ���ѹ���״̬
    if((SYS_Chr_State != Chr_State_sup)&&(DCDC_Val.Vol >= (DCDC_Val.NumofBatCells*4.20)))
    {
        SYS_Chr_State = Chr_State_CV;
    }
    //���ں�ѹ�׶Σ����������־��������
    if((SYS_Chr_State == Chr_State_CV)&&(DCDC_Val.Cur <= FinishCur))
    {
        SYS_Chr_State = Chr_State_Finish;
        Finish_min = Timer_IT_flags._1min;//��¼��ǰ����ʱ��
    }
    //�����ɺ�5���Ӻ󣬲��ҵ�ѹС����������ֵ����ʼ������
    if((SYS_Chr_State == Chr_State_Finish)&&(((Timer_IT_flags._1min - Finish_min) >= 5))&&(DCDC_Val.Vol < TriVol_th))
    {
        CTRL_DCDC_ON_OFF = DCDC_ON;
        SYS_Chr_State = Chr_State_trickle;
    }
    /********
    �����״̬���ж�Ӧ����ʲô���׶�END
    ********/
    
    
    
    if(SYS_Chr_State == Chr_State_sup)
    {
        //������
        CTRL_DCDC_CC_CV = DCDC_CC;
        delay_ms(30);
        SetDAC_Value((FinishCur*0.185+2.5)*10.0/16.8);//��FinishCur����������
    }
    if((SYS_Chr_State == Chr_State_CC)||(SYS_Chr_State == Chr_State_trickle))//�жϵ�ѹ����Ӧ���ں������״̬(������������ͺ���)
    {
        //�趨����״̬�Լ���Ӧ����
        //����ֵ�����趨ֵ
        CTRL_DCDC_CC_CV = DCDC_CC;//��֤���������в�����
        delay_ms(30);
        Sys_AutoRun_DC_Output();//���,�趨��Ӧ��������
        //DCDC_Val.SetVol = 4.2*DCDC_Val.NumofBatCells;//��ѹ�趨�����
    }
    if(SYS_Chr_State == Chr_State_CV)
    {
        //�趨����״̬�Լ���Ӧ����
        //��ѹֵ����ƽ��ģ���ȡ�м��ڵ�ض��������ѹ
        CTRL_DCDC_CC_CV = DCDC_CV;
        delay_ms(30);
        SetDAC_Value((DCDC_Val.ChrVol/101.0)*10.0);
    }
    if(SYS_Chr_State == Chr_State_Finish)
    {
        CTRL_DCDC_CC_CV = DCDC_CV;
        delay_ms(30);
        //SetDAC_Value((0/101.0)*10.0);//����������������ѹΪ0
        CTRL_DCDC_ON_OFF = DCDC_OFF;
    }
    
}

//�˵�ѡ���Դģʽ����������������
void Sys_AutoRun_DC_Output(void)
{
    float SetDAC_Value_CV,SetDAC_Value_CC,SetDAC_Temp;
    switch(CTRL_DCDC_CC_CV)
    {
        case DCDC_CV:
            // xΪʵ�ʵ�ѹ��yΪ������ѹ
            SetDAC_Value_CV = ((DCDC_Val.SetVol - 0.04)/101.0)*10.0;//��ѹ���贫�У�������
            SetDAC_Value_CV /= 1.1;//�˲�������
            SetDAC_Value(SetDAC_Value_CV);
            break;
        case DCDC_CC:
            //y = 0.3974x - 0.0112  xʵ�ʵ�����yΪINA194�����ѹ
            //y = 0.0973x2 + 0.7926x + 0.1542 xΪʵ�ʵ��� yΪ�趨����
            //����ʵ�������������������ۼ���Ƚϲ�����
            //SetDAC_Value_CC = (DCDC_Val.SetCur*0.185+2.5)*10.0/16.8;//��������оƬ����ACS712-05B
            //SetDAC_Value_CC /= 1.1;
            SetDAC_Temp = DCDC_Val.SetCur*DCDC_Val.SetCur*0.0973 + DCDC_Val.SetCur*0.7926 + 0.1542;
            SetDAC_Value_CC = SetDAC_Temp*0.3974 - 0.0112;
            SetDAC_Value_CC /= 1.1;
            SetDAC_Value(SetDAC_Value_CC);
            break;
        default:break;
    }
}

//��ȡDCDC��ѹ������ƽ��ģ��ĵ�ѹ�Լ��豸��MCU�¶ȣ��������������ã�
//���������ˢ����������ƽ�ȵķ�����WHILE���棬��������
void Sys_ReadinALLInf(void)
{
    //��74HC373����CD4051ѡ���ȡģ����
    Sel_CD4051_Ch(ch_Cur);
    delay_ms(5);//��Ҫ����ʱ
    DCDC_Val.Cur = (Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0;
    DCDC_Val.Cur = (DCDC_Val.Cur + 0.0112)/0.3974;//����
    //y = 0.0924x3 - 0.4267x2 + 1.5327x - 0.271
    DCDC_Val.Cur = 0.0924*DCDC_Val.Cur*DCDC_Val.Cur*DCDC_Val.Cur - 0.4267*DCDC_Val.Cur*DCDC_Val.Cur + 1.5327*DCDC_Val.Cur - 0.271;//����
    if(DCDC_Val.Cur < 0){DCDC_Val.Cur = 0;}
    
    Sel_CD4051_Ch(ch_Vol);
    delay_ms(5);
    DCDC_Val.Vol = ((Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0)/10.0*101.0;
    DCDC_Val.Vol = DCDC_Val.Vol + 0.3;//����
    if(DCDC_Val.Vol < 1.5){DCDC_Val.Vol = 0;}
    
    //������ֻ���¶ȵ���������������ϴ���LCD��ʾ�¶������ֶ���С��
    if(Error_code != Error_ds18b20_isn_init)
    {
        Temperature_DEV = (DS18B20_Get_Temp()/10);//ÿ100ms��ȡ������¶�
    }
    //Temperature_MCU = (Get_Temprate()/100);//�õ�MCU�¶�ֵ���������֣��ڲ�����ֻ���ţ�
    
    
    if(CTRL_DCDC_Mode == DCDC_Mode_Chr)
    {
        
//        Sel_CD4051_Ch(ch_C1);
//        DCDC_Val.BATCellVol_1 = (Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0;
//        
//        Sel_CD4051_Ch(ch_C2);
//        DCDC_Val.BATCellVol_2 = (Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0;
//        
//        Sel_CD4051_Ch(ch_C3);
//        DCDC_Val.BATCellVol_3 = (Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0;
//        
//        Sel_CD4051_Ch(ch_C4);
//        DCDC_Val.BATCellVol_4 = (Get_Adc_Average(MCU_AD_MUL_Ch,Adc_Average_times)*3.3)/4096.0;
        
        //�ж��м��ڵ��������ƽ��ͷ
        decideBatCells();
        //ƽ�⿪���رտ���
        Balance_ctrl();
    }
}

//����12λDAC������Ҫ�趨�ĵ�ѹֵС��(С��λ���ɶ�����λ)
void SetDAC_Value(float DAC_Value)
{
    /*
    b15 ��B����A
    b14 �޹�
    b13 ������1����2
    b12 ��������͹�ͨ��
    */
    
    u16 Digit_Out;
    DAC_Value -= 0.009;//����
    if(DAC_Value > 2){DAC_Value = 2.0;}else if(DAC_Value < 0){DAC_Value = 0;}
    Digit_Out = (u16)((DAC_Value*4096)/2.048);
    Digit_Out &= 0x0FFF;//�����λ
    Digit_Out |= 0x7000;//����λд0111
    SPI1_WriteByte(Digit_Out);
}

void initMCP4822(void)
{
    SPI1_WriteByte(0xE000);//�ر�Bͨ��
    
    if(CTRL_DCDC_CC_CV == DCDC_CV)
    {
        Sys_AutoRun_DC_Output();
    }
    
}


//ѡ��CD4051ģ�⴫��ͨ����ƽ��ģ��ŵ����
void Sel_CD4051_Ch(u8 ch)
{
    //˳�� C B A  ���� A2 A1 A0
    //PB12 13 14 ��Ӧ A B C
    PBout(6) = 1;//��ѡLCD
    switch(ch)
    {
        case ch_C0:GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);//000
            break;
        case ch_C4:GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);GPIO_SetBits(GPIOB,GPIO_Pin_14);//100
            break;
        case ch_C2:GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_14);GPIO_SetBits(GPIOB,GPIO_Pin_13);//010
            break;
        case ch_Cur:GPIO_ResetBits(GPIOB,GPIO_Pin_12);GPIO_SetBits(GPIOB,GPIO_Pin_14|GPIO_Pin_13);//110
            break;
        case ch_C1:GPIO_ResetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14);GPIO_SetBits(GPIOB,GPIO_Pin_12);//001
            break;
        case ch_C3:GPIO_ResetBits(GPIOB,GPIO_Pin_14);GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);//011
            break;
        case ch_Vol:GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);//111
            break;
        default:break;
    }
    _373_CS = _373_EN;//���߸������
    delay_ms(5);
    _373_CS = _373_disEN;
}

void Balance_ctrl(void)
{
    //�Ƚ�ÿ�ڵ�ص�ѹ��ֻ������һ�ڷŵ磬����ѹ���в������ĸ������ŵ�
    switch(DCDC_Val.NumofBatCells)
    {
        case 2:if(DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_1){
        DCDC_Val.BATCellDisChr_2 = 1;DCDC_Val.BATCellDisChr_1 = 0;
        }else{DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_1 = 0;}
            break;
        
        case 3:if((DCDC_Val.BATCellVol_3 > DCDC_Val.BATCellVol_1)&&(DCDC_Val.BATCellVol_3 > DCDC_Val.BATCellVol_2)){
            DCDC_Val.BATCellDisChr_3 = 1;DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }else if((DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_1)&&(DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_3)){
            DCDC_Val.BATCellDisChr_2 = 1;DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }else if((DCDC_Val.BATCellVol_1 > DCDC_Val.BATCellVol_2)&&(DCDC_Val.BATCellVol_1 > DCDC_Val.BATCellVol_3))
        {
            DCDC_Val.BATCellDisChr_1 = 1;DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_3 = 0;
        }else{DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_1 = 0;}
            break;
        
        case 4:if((DCDC_Val.BATCellVol_4 > DCDC_Val.BATCellVol_1)&&(DCDC_Val.BATCellVol_4 > DCDC_Val.BATCellVol_2)&&(DCDC_Val.BATCellVol_4 > DCDC_Val.BATCellVol_3))
        {
            DCDC_Val.BATCellDisChr_4 = 1;DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }else if((DCDC_Val.BATCellVol_3 > DCDC_Val.BATCellVol_1)&&(DCDC_Val.BATCellVol_3 > DCDC_Val.BATCellVol_2)&&(DCDC_Val.BATCellVol_3 > DCDC_Val.BATCellVol_4))
        {
            DCDC_Val.BATCellDisChr_3 = 1;DCDC_Val.BATCellDisChr_4 = 0;DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }else if((DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_1)&&(DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_3)&&(DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_4))
        {
            DCDC_Val.BATCellDisChr_2 = 1;DCDC_Val.BATCellDisChr_4 = 0;DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }else if((DCDC_Val.BATCellVol_1 > DCDC_Val.BATCellVol_2)&&(DCDC_Val.BATCellVol_1 > DCDC_Val.BATCellVol_3)&&(DCDC_Val.BATCellVol_2 > DCDC_Val.BATCellVol_4))
        {
            DCDC_Val.BATCellDisChr_1 = 1;DCDC_Val.BATCellDisChr_4 = 0;DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_2 = 0;
        }else{
            DCDC_Val.BATCellDisChr_2 = 0;DCDC_Val.BATCellDisChr_4 = 0;DCDC_Val.BATCellDisChr_3 = 0;DCDC_Val.BATCellDisChr_1 = 0;
        }
            break;
        default:break;
    }
    
    //���Ʒŵ�
    PBout(6) = 1;//��ѡLCD
    if(DCDC_Val.BATCellDisChr_1){Balance_1 = Balance_ON;}else{Balance_1 = Balance_OFF;}
    if(DCDC_Val.BATCellDisChr_2){Balance_2 = Balance_ON;}else{Balance_2 = Balance_OFF;}
    if(DCDC_Val.BATCellDisChr_3){Balance_3 = Balance_ON;}else{Balance_3 = Balance_OFF;}
    if(DCDC_Val.BATCellDisChr_4){Balance_4 = Balance_ON;}else{Balance_4 = Balance_OFF;}
    _373_CS = _373_EN;//���߸������
    delay_ms(5);
    _373_CS = _373_disEN;
}

void decideBatCells(void)
{
    if(CTRL_DCDC_Mode == DCDC_Mode_Chr)
    {
        if(DCDC_Val.BATCellVol_1 < 1){DCDC_Val.NumofBatCells = 0;}
        else if(DCDC_Val.BATCellVol_2 < 1){DCDC_Val.NumofBatCells = 1;}
        else if(DCDC_Val.BATCellVol_3 < 1){DCDC_Val.NumofBatCells = 2;}
        else if(DCDC_Val.BATCellVol_4 < 1){DCDC_Val.NumofBatCells = 3;}
        else{DCDC_Val.NumofBatCells = 4;}
        DCDC_Val.ChrVol = DCDC_Val.NumofBatCells*4.20;
    }
    
    
}



