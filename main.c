/**
  ******************************************************************************
  * @file    main.c
  * @author  luziqi
  * @version Beta-0.8
  * @date    2018-1-8
  * @brief   ���ܳ���Ͷι����
  ******************************************************************************
  * @attention
  *
  * ����ʵ��
  * 		�������flash�ж�ȡͶι�ƻ���Ȼ�����ESP8266��������ģ�鳢����������
	*	ͨ�����������ͨѶ,��ȡ�������ݿ��е�Ͷι�ƻ���Ϣ��ͬ�����µ�����flash�У�
	*	����TIMָ��ϵͳʱ�ӣ�����Ͷι�ƻ���Ϣ����ָ����ʱ�䣬�����������ģ�飬
	*	�����ų���������������ش������ﵽҪ��������ֵ��ʧ�ܼ������ģ�飬ֹͣ
	*	�ų��������ﵽ����ʳ����Ŀ�ġ�
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "bsp_dht11.h"
//#include "spi_flash.h"
#include <stdio.h>  
#include <string.h> 

#include "test.h"
#include "network.h"
#include "flash.h"
#include "rtc.h"
#include "linked_list.h"
#include "device.h"
#include "HX711.h"
#include "tim2.h"
#include "led.h"

pNode pList = NULL;

uint8_t *Rx_Data;
uint8_t Tx_Data[1024];
char* clientID;
 
char ch;

int w;

/* �������� */
void rtc_Init(int);
void Delay(__IO uint32_t nCount);



int main ( void )
{
	int ifu;

	
	//��ʼ��
	led_Init();
  USARTx_Config ();                                                           
	SysTick_Init ();                                                              
	ESP8266_Init (); 
	rtc_Init(1);	
	tim2_Nvic();
	tim2_Init();
	motor_init();
	//motor_status(1);	
	Init_HX711pin();
	NVIC_Configuration(); 
	
	
	
	
	Get_Maopi();	
	
	//��ȡFlash����
	Rx_Data = getFlashData();	
	//setFlashData((uint8_t *)"id,1150134,init,0,ap_name,zzzzzzz,ap_pwd,88888888,feedplan,3,diet,27000.80.43200.120.64830.100.,");
	ifu = isFirstUse();
	//��ȡ�豸id
	clientID = getClientID();
	printf("\r\n[Main]%s\r\n", clientID);	
	//��������
	pList = list_saveDiet(flash_getData("diet"));
	//���ӷ�����
	printf("[Main]ifu = %d", ifu);
  net_linkToServer(ifu);
	//���Ϳͻ�ID
	//while(!ESP8266_SendString ( DISABLE, clientID, strlen ( clientID ), Single_ID_0)){}
	
	
	Get_Weight();
	//ȡ��ѹ����������ʼֵ
	w = Weight_Shiwu;
	tim2_Start();
	//motor_status(1);
	//apk_flag = 1;
	
  while ( 1 ){
		
		net_bind();

		//apk_flag = 1;
		//plan_Action();

	};
}



//ϵͳʱ�ӳ�ʼ������
void rtc_Init(int flag){
	//RTC_NVIC_Configuration();
	
	/* ����Ƿ��һ������ */
	//if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5){
	if(!flag){
		printf("\r\n[RTC]RTCû������\r\n");
		RTC_Configuration();
		printf("\r\n[RTC]��ʼ����RTC\r\n");
		
		//����ʱ�亯��
		Time_Adjust();
		//д���ʼ����־
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		
	}else{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET){
			printf("\r\n[RTC]Power On Reset occurred....\r\n");
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){
			printf("\r\n[RTC]External Reset occurred....\r\n");
		}
		printf("\r\n[RTC]RTC������,����������....\r\n");
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
		/* Enable the RTC Second */
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
	
	#ifdef RTCClockOutput_Enable
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); 
	/* To output RTCCLK/64 on Tamper pin, the tamper functionality must be disabled */
	
	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
	#endif
	
	/* Clear reset flags */
	RCC_ClearFlag();
	/* �ڴ���ͨѶ����ʾʱ����Ϣ */
	//Time_Show();
	printf("\r\n[RTC]��ʼ�����(%d)\r\n", Time_GetCounter());
}

void Delay(__IO uint32_t nCount){
  for(; nCount != 0; nCount--);
}



/*********************************************END OF FILE**********************/
