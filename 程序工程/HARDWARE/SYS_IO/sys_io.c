#include "sys_io.h"
#include "delay.h"
/*
����������ģ�黯��Ϊ����Ŀר�Ŷ��Ƶ����蹦�ܣ�
DCDCоƬ���ؿ����ߡ���PA8--�ߵ�ƽ�ر�DCDCоƬ
������ѹģʽ�����ߡ���PA1--�͵�ƽ���ú�������ģʽ����֮��ѹ	
���򰴼�ADKey����AD�ɸߵ��ͣ����������ң�ADC1_IN8->ADKey
373������ʹ�ܡ���PB1->373EN����������չ��ƽ��ģ���ĸ�MOS��4051�ĵ�ַABC
�豸�¶ȡ���PA15->DS18B20
����4051�Ķ��ģ������ADC_IN0->MulVol��ͨ��0��4��C0��C4��ͨ��6��7��DCDC�ĵ����͵�ѹ

SPI(һ�����ʹ��)
MOSI-PA7��MISO-PA6��SCK-PA5��SS-PA4(DAоƬ)

����2�ĳ�ʼ������TX-PA2��RX-PA3��ͨ��˫��˫������ѡ�����ESP8266-07�Ĵ���1��������������
*/


void SYS_IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|\
	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|\
	RCC_APB2Periph_GPIOC, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    
	//STEP��KEY�ⲿ�жϳ�ʼ��
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
//	
//	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line13;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	//GPIOA��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //SPI1���ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;//SCK MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//SS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_SetBits(GPIOA,GPIO_Pin_4);//���
    
    //SPI1��ʼ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;//ֻ���Ͳ�����
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//ʱ�Ӽ��ԣ�����ʱ�͵�ƽ��������ʱ�������ش���ӻ�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//ʱ����λ����һ��ʱ�������ؿ�ʼ���䣬����MCP4822��Э��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//ʹ������������ã���Ӳ��
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//36M/256=140.625KHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1,&SPI_InitStructure);
    SPI_Cmd(SPI1,ENABLE);
    
    //SPI1_ReadWriteByte(0xff);//�������þ���ά��MOSIΪ�ߵ�ƽ��������仰Ҳ���Ǳ����
    
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);//�ر�DCDC
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//�趨��ѹMODE
	
	//GPIOB��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //�ڴ�֮ǰ������������LCD��IO������LCD_Init()�ŵ�SYS_IO_Init()ǰ�棡
    
    GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);//�ر���·ƽ��PMOS
    GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);//4051��ַѡ000
    GPIO_SetBits(GPIOB,GPIO_Pin_1);//����373������
    delay_ms(5);
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);//����373����ˣ��ر�������
}



void TestLED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

//ͨ��SPI1�ڷ���һ�����ݣ�����1��ʾ�ɹ�
u8 SPI1_WriteByte(u16 TxData)
{
    u8 retry=0;
    u16 tx_buf;
    SPI_SS = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //�ȴ���������
    {
        retry++;
        if(retry>200)return 0;
    }
    
    tx_buf = TxData;
    delay_ms(1);
    SPI_I2S_SendData(SPI1, (u8)(tx_buf >> 8)); //ͨ������ SPIx ��������
    delay_ms(1);
    SPI_I2S_SendData(SPI1, (u8)TxData);
    delay_ms(1);
    SPI_SS = 1;
    return 1;
}

//ͨ��SPI1�ڷ���һ�����ݣ�ͬʱ����һ������
//u8 SPI1_ReadWriteByte(u8 TxData)
//{
//    u8 retry=0;
//    SPI_SS = 0;
//    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //�ȴ���������
//    {
//        retry++;
//        if(retry>200)return 0;
//    }
//    SPI_I2S_SendData(SPI1, TxData); //ͨ������ SPIx ����һ������
//    retry=0;
//    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ�����
//    { 
//        retry++;
//        if(retry>200)return 0;
//    }
//    SPI_SS = 1;
//    return SPI_I2S_ReceiveData(SPI1); //����ͨ�� SPIx ������յ�����
//}




