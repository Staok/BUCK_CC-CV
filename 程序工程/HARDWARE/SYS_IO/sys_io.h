#ifndef __SYS_IO_H
#define __SYS_IO_H	 
#include "sys.h"
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

#define IO_TestLED PBout(12)
#define SPI_SS PAout(4)


void SYS_IO_Init(void);
void TestLED_init(void);
//u8 SPI1_ReadWriteByte(u8 TxData);
u8 SPI1_WriteByte(u16 TxData);
		 				    
#endif

