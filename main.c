/**
  ******************************************************************************
  * @file    main.c
  * @author  luziqi
  * @version Beta-0.8
  * @date    2018-1-8
  * @brief   智能宠物投喂机器
  ******************************************************************************
  * @attention
  *
  * 功能实现
  * 		开机后从flash中读取投喂计划，然后借由ESP8266无线网络模块尝试连接网络
	*	通过与服务器的通讯,获取网络数据库中的投喂计划信息，同步更新到本机flash中，
	*	利用TIM指定系统时钟，根据投喂计划信息，在指定的时间，驱动减速马达模块，
	*	缓慢放出宠物干粮，当称重传感器达到要求重量数值后，失能减速马达模块，停止
	*	放出干粮，达到控制食量的目的。
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

/* 函数声明 */
void rtc_Init(int);
void Delay(__IO uint32_t nCount);



int main ( void )
{
	int ifu;

	
	//初始化
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
	
	//获取Flash数据
	Rx_Data = getFlashData();	
	//setFlashData((uint8_t *)"id,1150134,init,0,ap_name,zzzzzzz,ap_pwd,88888888,feedplan,3,diet,27000.80.43200.120.64830.100.,");
	ifu = isFirstUse();
	//获取设备id
	clientID = getClientID();
	printf("\r\n[Main]%s\r\n", clientID);	
	//创建链表
	pList = list_saveDiet(flash_getData("diet"));
	//连接服务器
	printf("[Main]ifu = %d", ifu);
  net_linkToServer(ifu);
	//发送客户ID
	//while(!ESP8266_SendString ( DISABLE, clientID, strlen ( clientID ), Single_ID_0)){}
	
	
	Get_Weight();
	//取得压力传感器初始值
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



//系统时钟初始化函数
void rtc_Init(int flag){
	//RTC_NVIC_Configuration();
	
	/* 检查是否第一次配置 */
	//if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5){
	if(!flag){
		printf("\r\n[RTC]RTC没有配置\r\n");
		RTC_Configuration();
		printf("\r\n[RTC]开始配置RTC\r\n");
		
		//调整时间函数
		Time_Adjust();
		//写入初始化标志
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
		printf("\r\n[RTC]RTC已配置,无需再配置....\r\n");
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
	/* 在串口通讯上显示时间信息 */
	//Time_Show();
	printf("\r\n[RTC]初始化完成(%d)\r\n", Time_GetCounter());
}

void Delay(__IO uint32_t nCount){
  for(; nCount != 0; nCount--);
}



/*********************************************END OF FILE**********************/
