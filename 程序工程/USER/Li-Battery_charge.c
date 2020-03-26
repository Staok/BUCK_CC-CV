//  ��������������������
//  ���������ߩ��������ߩ�
//  ����������������������
//  ����������������������
//  �����������ס��ש�����
//  ����������������������
//  �������������ߡ�������
//  ������������������  ��
//  ����������������������Codes are far away from bugs with the Mysterious Animal protecting
//  ������������������    ���ޱ���,������bug
//  ������������������
//  ��������������������������
//  ����������              �ǩ�
//  ����������              ����
//  �������������������ש�����
//  �������������ϩϡ����ϩ�
//  �������������ߩ������ߩ�
#include "sys.h"
#include "delay.h"
#include "beep.h"
#include "ds18b20.h" 
#include "timer.h"
#include "usart.h"
#include "sys_io.h"
#include "wdg.h"
#include "tsensor.h"
#include "adc.h"
#include "lcd.h"
#include "GUI.h"

#include "SelfChecking.h"
#include "MENU.h"
#include "SYS_INF_CTRL.h"

//DS18B20��ȡ����һ������С������

/*********
�ж����ȼ�˵����
��ռ���ȼ�������ʱ��(TIM3)�ж�(1) > ���ڽ����ж�(2) > TIM2�����������ж�(3)
*********/

/********
LCDTFT�������˵����
�����м���л���Դ����ƽ���ģʽ��
�̰��м���л��ֵ�����ϸ����
�����л�������Ŀ�����ҵ��òΣ�
ƽ���ģʽ�£�����״̬���������ɵ���
********/

#define USART1_bound 9600
#define USART2_bound 115200

//�Ƿ���Ӧ����
#define ResponseFault 0
#define CheckFault 0

//�����ļ���������


int main(void)
{
	static u8 Error_check_now = 1;
    //float i = 1;
    static u8 RunOnce = 1;
//		u16 len;
    delay_init();
    LCD_Init();
	SYS_IO_Init();
	//����Ĭ��ģʽ�趨��DCDC�رա���ѹ���ڡ���Դģʽ
	Sys_ParameterInit();
	uart_init(USART1_bound);
	//T_Adc_Init();//����
    Adc_Init();
    initMCP4822();//�ر�DACоƬ��Bͨ�������趨���Aͨ��DA
    BEEP_Init();
    TIM2_Int_Init();
    TestLED_init();
    
	if(CTRL_DCDC_Mode == DCDC_Mode_DC)
	{
		DrawPowerModePage();
	}else if(CTRL_DCDC_Mode == DCDC_Mode_Chr)
	{
		DrawChargeModePage();
	}
    
	delay_ms(369);
	if(DS18B20_Init())	//DS18B20��ʼ��	
	{
		Error_code = Error_ds18b20_isn_init;
	}
    
    Sys_ReadinALLInf();//������ȡһ������
    is_DCDC_Output_Normal();//�ϵ��Լ�
	//is_Tem_Over_Scope();
    
    Draw_Refresh_All();//ˢ�½���
    
	delay_ms(369);
	TIM3_Int_Init();//Ĭ��ѡ��10MS��ʱ�ж�
    delay_ms(119);
	IWDG_Init();//Ĭ��ѡ����ÿ1Sι��
    
//    SetDAC_Value(0.90009);
    
//    PBout(6) = 1;//��ѡLCD
//    Balance_1 = Balance_ON;
//    Balance_2 = Balance_OFF;
//    Balance_3 = Balance_ON;
//    Balance_4 = Balance_OFF;
//    _373_CS = _373_EN;//���߸������
//    delay_ms(5);
//    _373_CS = _373_disEN;
//    
//    
//    Sel_CD4051_Ch(ch_C3);

//    while(1){;}
//   while(1)
//   {
//       for(i = 170;i > 30;i--)
//       {
//           SetDAC_Value(i/100.0);//SetDAC_Value����ֵ���ܳ���1.8,���ڵ�ѹ 1.8*1.1/10*(10+91) = 20.0
//           delay_ms(300);
//       }
//   }
    buzzer_once;//��һ����ʾϵͳ��ʼ�����
	while(1)
	{
		/*********
		������Ӧ����START
		*********/
		if(ResponseFault)
		{
			switch(Error_code)//�����׳�����ʱ������Ĺ��Ϸ���ǰ�棬������Ҫ������
			{
				case Error_DCDC_Output_isn_Normal://���������ϣ�����whileѭ���ж�������Լ��еõ�
					dealWith_Output_isn_Normal();
					break;
				case Error_DCDC_isn_shut://���������ϣ����ڳ���ر�DCDCоƬ���Լ�õ�
					dealWith_Output_isn_Normal();//�������Ƿ�رչ��ϵĺ���ת�Ƶ��������Ƿ�����������
					break;
				case Error_Tem_Over_Scope://���������ϣ�����whileѭ���жԵ�����MCU�¶ȵ��Լ��еõ�
					dealWith_Tem_Over_Scope();
					break;
				case Error_ds18b20_isn_init://���������ϣ������ϵ��DS18B20��ʼ�����Լ��еõ�
					dealWith_ds18b20_isn_init();
					break;
				case 0:break;
			}
		}
		/*********
		������Ӧ����END
		*********/
        
		if(Timer_IT_flags._1sec_flag == 1)
		{
			Timer_IT_flags._1sec_flag = 0;
			//IO_TestLED = !IO_TestLED;//�Ʊ�LCD����
            
			/*********
			ˢ����Ļ�ϵ�����ָʾ��
			*********/
			Draw_Refresh_PowSignal();
		}
		
		if(Timer_IT_flags._300msec_flag == 1)
		{
			Timer_IT_flags._300msec_flag = 0;
			
            if(CheckFault)
            {
                /*********
                ���й���Ѳ��START
                *********/
                //��Ҫд���ж���䣬����ռ��̫��ʱ��
                switch(Error_check_now)
                {
                    case 1:is_DCDC_Output_Normal();break;
                    case 2:is_Tem_Over_Scope();break;
                    default:break;
                }
                if(++Error_check_now > Error_check_num)
                {
                    Error_check_now = 1;
                }
                /*********
                ���й���Ѳ��END
                *********/
            }

            /*********
			����������Ϣ
			*********/
            Sys_ReadinALLInf();
            /*********
			ˢ�½�����Ϣ
			*********/
			Draw_Refresh_All();
            
		}
		
        //��������
        if(Timer_IT_flags._100msec_flag == 1)
        {
            Timer_IT_flags._100msec_flag = 0;
            
            DCDC_Val.AD5Key_Value = Get_Adc_Average(MCU_AD_5Key_Ch,2);
            
            if(DCDC_Val.AD5Key_Value > 3500)//���Σ��ú�������,����Key_UseUp = 1;
            {
                RunOnce = 1;
                if(!Key_UseUp)
                {KeyPos = Key_Down;
                buzzer_once;
                }
            }else if(DCDC_Val.AD5Key_Value > 3020)
            {
                RunOnce = 1;
                if(!Key_UseUp)
                {KeyPos = Key_Left;
                //buzzer_once;
                }
            }else if(DCDC_Val.AD5Key_Value > 2640)
            {
                RunOnce = 1;
                if(!Key_UseUp)
                {KeyPos = Key_Up;
                buzzer_once;
                }
            }else if(DCDC_Val.AD5Key_Value > 2330)
            {
                RunOnce = 1;
                if(!Key_UseUp)
                {KeyPos = Key_Center;
                buzzer_once;
                }
                
                if(++Key_CenterCount > 15)//����
                {
                    Key_CenterCount = 0;
                    is_Key_Center_LongPrass = 1;
                }
            }else if(DCDC_Val.AD5Key_Value > 1830)
            {
                RunOnce = 1;
                if(!Key_UseUp)
                {KeyPos = Key_Right;
                //buzzer_once;
                }
            }else {if(RunOnce == 1){clearAllKeyInf();RunOnce = 0;}}
            
            
            if(is_Key_Center_LongPrass)//�л�ģʽ
            {
                is_Key_Center_LongPrass = 0;
                CTRL_DCDC_ON_OFF = DCDC_OFF;
                delay_ms(10);
                
                if(CTRL_DCDC_Mode == DCDC_Mode_DC)
                {
                    CTRL_DCDC_Mode = DCDC_Mode_Chr;
                    CTRL_DCDC_CC_CV = DCDC_CC;
                    DrawChargeModePage();
                }else{
                    CTRL_DCDC_Mode = DCDC_Mode_DC;
                    CTRL_DCDC_CC_CV = DCDC_CV;
                    DrawPowerModePage();
                }
                MENU_Refrash_Pos();
                buzzer_once;
            }
            
            switch(CTRL_DCDC_Mode)
            {
                case DCDC_Mode_DC:Opera_DC_Menu();break;
                case DCDC_Mode_Chr:Opera_Chr_Menu();break;
                default:break;
            }
        }
        
        
        //�����趨����ʱ�䣬��������
		while(_OutofTime_Running_flag){
            CTRL_DCDC_ON_OFF = DCDC_OFF;
            DrawPageHead("TIME OUT!Please shutdown.");
			CTRL_buzzer_ON_OFF = Buzzer_ON;
            while(1){;}}
		      
        //����1�����ã��ش���������Ϣ
//		if(USART_RX_STA&0x8000)
//		{					   
//			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
//			printf("\r\n�����͵���ϢΪ:\r\n");
//			printf("\r\n%s\r\n",USART_RX_BUF);
//			printf("\r\n�����͵���Ϣ����Ϊ:\r\n");
//			printf("\r\n%d\r\n",len);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
////			for(t=0;t<len;t++)
////			{
////				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
////				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
////			}
//			printf("\r\n\r\n");//���뻻��
//			USART_RX_STA=0;
//		}
		
		
	}
}


