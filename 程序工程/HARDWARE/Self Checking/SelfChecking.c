#include "SelfChecking.h"
#include "delay.h"
#include "timer.h"
#include "beep.h"
#include "ds18b20.h"
#include "tsensor.h"
#include "SYS_INF_CTRL.h"


u8 Error_code = 0;

u16 ShutSecondNow = 0;

/*********
�Լ캯��START
*********/
//�Լ�DCDC�Ƿ������رգ������ڳ���ر�DCDCоƬ֮����Լ죬�ڹر������ֻдShutSecondNow = Timer_IT_flags._1sec;���ɣ�
//u8 IS_DCDC_OFF(void)
//{
//    
//	//CTRL_DCDC_ON_OFF = DCDC_OFF;
//	//delay_ms(5000);//��ʱ����ʵ������޸ģ�Ӧ��������ѹ���͵�1V�����ٳ�һ���
//    if(((Timer_IT_flags._1sec - ShutSecondNow) > 5)&&(CTRL_DCDC_Mode == DCDC_Mode_DC))
//    {
//        if((DCDC_Val.Cur > 0.1)&&(DCDC_Val.Vol > 1))//������г����Ľϴ�������ѹ����ʾû��DCDC�ص�
//        {
//            Error_code = Error_DCDC_isn_shut;//ֻ���ڴ�����Ϻ�������Ѵ��������0���޹��ϣ�����⺯��ֻ��������ɾ��
//            return 0;
//        }else{ return 1;}//����1��ʾ�ɹ����޴�
//    }
//    return 1;
//}
//�Լ��¶��Ƿ���������Χ
u8 is_Tem_Over_Scope(void)
{
//	if((Temperature_DEV < 5)||(Temperature_DEV > 40) || (Temperature_MCU < 5)||(Temperature_MCU > 50))
//	{
//		Error_code = Error_Tem_Over_Scope;
//		return 0;
//	}else{ return 1;}
	if((Temperature_DEV < 5)||(Temperature_DEV > 40))
	{
		Error_code = Error_Tem_Over_Scope;
		return 0;
	}else{ return 1;}
}
//�Լ쵱DCDCоƬ����ʱ�����ж����ڴ��ں������Ǻ�ѹģʽ���ٸ����ж�����Ƿ�����
u8 is_DCDC_Output_Normal(void)
{
	if(CTRL_DCDC_ON_OFF == DCDC_ON)
	{
		switch(CTRL_DCDC_Mode)//ѹ�����������ޣ�ѹ������С���趨ֵ
		{
			case DCDC_Mode_DC:
				if((DCDC_Val.Cur > DCDC_Val.Cur_Limit + 0.1)||(DCDC_Val.Vol > DCDC_Val.Vol_Limit + 1))
				{
					Error_code = Error_DCDC_Output_isn_Normal; //�����ѹ�쳣����־�������
					return 0; //���ش���
				}
                if((DCDC_Val.Cur < DCDC_Val.SetCur-0.5)||(DCDC_Val.Vol < DCDC_Val.SetVol-1))
                {
                    Error_code = Error_DCDC_Output_isn_Normal;
                    return 0;
                }
				break;
			case DCDC_Mode_Chr:
				if((DCDC_Val.Cur > DCDC_Val.Cur_Limit + 0.1)||(DCDC_Val.Vol > (4.2*DCDC_Val.NumofBatCells+0.2))) //+xx��ԣ��
				{
					Error_code = Error_DCDC_Output_isn_Normal;
					return 0;
				}
                //����ڲ�����״̬����������С���趨������һ��
                if((SYS_Chr_State == Chr_State_sup)&&(DCDC_Val.Cur < FinishCur/2.0))
                {
                    Error_code = Error_DCDC_Output_isn_Normal;
                    return 0;
                }
                //����ں�������������״̬����������С���趨ֵ-0.5
                if(((SYS_Chr_State == Chr_State_CC)||(SYS_Chr_State == Chr_State_trickle))&&(DCDC_Val.Cur < DCDC_Val.SetCur-0.5))
                {
                    Error_code = Error_DCDC_Output_isn_Normal;
                    return 0;
                }
				break;
			default:break;
		}
        return 1;//��������
	}else if(CTRL_DCDC_ON_OFF == DCDC_OFF)
    {
        //CTRL_DCDC_ON_OFF = DCDC_OFF;
        //�Լ�DCDC�Ƿ������رգ������ڳ���ر�DCDCоƬ֮����Լ죬�ڹر������ֻдShutSecondNow = Timer_IT_flags._1sec;����
        if(((Timer_IT_flags._1sec - ShutSecondNow) > 5)&&(CTRL_DCDC_Mode == DCDC_Mode_DC))
        {
            if((DCDC_Val.Cur > 0.1)&&(DCDC_Val.Vol > 1))//������г����Ľϴ�������ѹ����ʾû��DCDC�ص�
            {
                Error_code = Error_DCDC_isn_shut;//ֻ���ڴ�����Ϻ�������Ѵ��������0���޹��ϣ�����⺯��ֻ��������ɾ��
                return 0;
            }else{ return 1;}//����1��ʾ�ɹ����޴�
        }
        return 1;
    }
    return 1;
}
/*********
�Լ캯��END
*********/




/*********
���ϴ�����START
*********/
//������ʽ���й���������������¶Ȳ������ģ������ر�DCDC�������Ե��������������
//��Ļ��ErrorCode��ʾ��ʲô���󣬳�����ÿ��300ms����1s�����¼�飬����ָ��������������������Ҫ�ֶ�����DCDC
void dealWith_Output_isn_Normal(void)
{
	CTRL_DCDC_ON_OFF = DCDC_OFF;
	CTRL_buzzer_ON_OFF = Buzzer_ON;
	//��ʾ��û�������������������Ŧ�ӿ��ش򿪷񣿿��������ֶ�������
//	while(Error_code)
//	{
		if(Timer_IT_flags._1sec_flag == 1)
		{
			//Timer_IT_flags._1sec_flag = 0;//��ѭ�������ĳ�������־
			//buzzer_once;
			if(is_DCDC_Output_Normal())
			{
				Error_code = 0;
				;//��Ļ������棬�ָ�����
				CTRL_buzzer_ON_OFF = Buzzer_OFF;
			}
		}
//	}
}
//void dealWith_DCDC_isn_Shut(void)
//{
//	CTRL_DCDC_ON_OFF = DCDC_OFF;
//	CTRL_buzzer_ON_OFF = Buzzer_ON;
//	//"û�������ر�������Ѿ�ͣ�����������ֶ��ػ�"
////	while(Error_code)
////	{
//		if(Timer_IT_flags._1sec_flag == 1)
//		{
//			//Timer_IT_flags._1sec_flag = 0;//��ѭ�������ĳ�������־
//			
//			if(IS_DCDC_OFF())
//			{
//				Error_code = 0;
//				;//��Ļ������棬�ָ�����
//				CTRL_buzzer_ON_OFF = Buzzer_OFF;
//			}
//		}
////	}
//}
//����DS18B20��ʼ������
void dealWith_ds18b20_isn_init(void)//��Ļ��ʾ����ѭ�����³�ʼ����ֱ����ʼ����ɾ�������ϱ�־
{
	//CTRL_DCDC_ON_OFF = DCDC_OFF;
	//CTRL_buzzer_ON_OFF = Buzzer_ON;
	;//��Ļ��ʾ����"�¶ȼ��ģ���ʼ��δ��ɣ��Ѿ�ͣ�������ڲ������ԣ��������ֶ�����"
	//while(Error_code)
	//{
		if(Timer_IT_flags._1sec_flag == 1)
		{
			//Timer_IT_flags._1sec_flag = 0;//��ѭ�������ĳ�������־
			//buzzer_once;
			if(DS18B20_Init())//DS18B20��ʼ��������1��ʾ����
			{
				Error_code = Error_ds18b20_isn_init;
			}else{ Error_code = 0;}//��Ļ������棬�ָ�����
		}
	//}
	//CTRL_buzzer_ON_OFF = Buzzer_OFF;
	//CTRL_DCDC_ON_OFF = DCDC_ON;
}
//���������¶���һ����Χ����
void dealWith_Tem_Over_Scope(void)//��Ļ��ʾ��ͬʱѯ���Ƿ����³�ʼ����ֱ����ʼ����ɾ�������ϱ�־
{
	CTRL_DCDC_ON_OFF = DCDC_OFF;
	CTRL_buzzer_ON_OFF = Buzzer_ON;
	;//��Ļ��ʾ����"�¶ȳ���Χ���Ѿ�ͣ�����������ֶ��ػ��ȴ��¶��½��ٿ���"
//	while(Error_code)
//	{
		if(Timer_IT_flags._300msec_flag == 1)
		{
			//Timer_IT_flags._300msec_flag = 0;//��ѭ�������ĳ�������־
			//buzzer_once;
			if(is_Tem_Over_Scope())
			{
				Error_code = 0;
				;//��Ļ������棬�ָ�����
				CTRL_buzzer_ON_OFF = Buzzer_OFF;
				//CTRL_DCDC_ON_OFF = DCDC_ON;
			}
		}
//	}

}
/*********
���ϴ�����END
*********/


