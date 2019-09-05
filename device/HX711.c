/************************************************************************************
							�������ṩ�����µ��̣�
								Ilovemcu.taobao.com
								epic-mcu.taobao.com
							ʵ�������Χ��չģ����������ϵ���
							���ߣ����زر���							
*************************************************************************************/
#include "HX711.h"
#include "delay.h"

u32 HX711_Buffer;
u32 Weight_Maopi;
s32 Weight_Shiwu;
u8 Flag_Error = 0;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 1870


void Init_HX711pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 	//ʹ��PF�˿�ʱ��

	//HX711_SCK
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 				// �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��GPIOB
	
	//HX711_DOUT
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;						//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	GPIO_SetBits(GPIOB,GPIO_Pin_0);													//��ʼ������Ϊ0
}



//****************************************************
//��ȡHX711
//****************************************************
u32 HX711_Read(void)	//����128
{
	unsigned long count; 
	unsigned char i; 
  HX711_DOUT=1; 
	//delay_us(1);
  HX711_SCK=0; 
  count=0; 
  while(HX711_DOUT); 
	for(i=0;i<24;i++){ 
	  HX711_SCK=1; 
	  count=count<<1; 
		//delay_us(1);
		HX711_SCK=0; 
	  if(HX711_DOUT)
			count++; 
		//delay_us(1);
	} 
 	HX711_SCK=1; 
    count=count^0x800000;										//��25�������½�����ʱ��ת������
	//delay_us(1);
	HX711_SCK=0;  
	return(count);
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//����
//****************************************************
void Get_Weight(void)
{
	HX711_Buffer = HX711_Read();
	if(HX711_Buffer > Weight_Maopi)			
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;						//��ȡʵ���AD������ֵ��
	
		Weight_Shiwu = (s32)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
																													//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
																													//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
																													//������Գ���������ƫСʱ����С����ֵ��
	}

	
}
