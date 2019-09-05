#include "stm32f10x.h"
#include <stdio.h>
#include "tim2.h"
#include "network.h"
#include "HX711.h"
#include "device.h"
#include "linked_list.h"
#include "rtc.h"

int apk_flag = 0;
int t;

//初始化
void tim2_Init(void){
  TIM_TimeBaseInitTypeDef tim;
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 , ENABLE );
	TIM_DeInit( TIM2 );
	tim.TIM_Period = 1000;//10000
	tim.TIM_Prescaler = ( 7200 - 1 );//7200
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &tim );
	TIM_ClearFlag( TIM2, TIM_FLAG_Update );	
}

void tim2_Start(void){
	TIM_ITConfig( TIM2,TIM_IT_Update, ENABLE );	
	TIM_Cmd(TIM2, ENABLE);	
	printf("[Tim]开始计时");
}

//中断管理
void tim2_Nvic(void){
	NVIC_InitTypeDef nvic;
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0 );
	
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 2;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nvic );
}

//中断函数
void TIM2_IRQHandler(void){
	//每100ms触发一次中断
	if( TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET ){
		t++;
		Get_Weight();
		if(t == 10){
			if( pList->timecount == Time_GetCounter() || (apk_flag == 1)){
				motor_status(1);
				apk_flag = 2;
			}
			t = 0;
		}
		if(Weight_Shiwu-w > pList->weight && Motor_Status && (apk_flag != 2)){
			motor_status(0);
		}
		if(Weight_Shiwu-w > pList->weight && (apk_flag == 2) ){
			motor_status(0);
			apk_flag = 0;
		}
		//printf("\r\n[TIM2]Hello!\r\n");
		
		TIM_ClearITPendingBit( TIM2, TIM_IT_Update );//清除中断
		
	}

}



