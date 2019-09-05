#include "device.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

int Motor_Status = 0;

//减速马达初始化设置
void motor_init(void){
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE); 
	gpio.GPIO_Pin = GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &gpio);	
	GPIO_ResetBits(GPIOD, GPIO_Pin_1);
}

//使能或失能减速马达
void motor_status(int flag){
	if(flag){
		GPIO_SetBits(GPIOD, GPIO_Pin_1);
		Motor_Status = 1;
	}else{
		GPIO_ResetBits(GPIOD, GPIO_Pin_1);
		Motor_Status = 0;
	}
}
