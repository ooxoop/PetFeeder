#include "stm32f10x.h"
#include <stdio.h>  

void led_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); 
													   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6  ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIOE*/
  GPIO_Init(GPIOE, &GPIO_InitStructure);		  
	/* 关闭所有led灯	*/
	GPIO_SetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6);	 
}

void led1_status(int s){
	if(!s){
		GPIO_SetBits(GPIOE, GPIO_Pin_5);	
	}else{
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);	
	}
}

void led2_status(int s){
	if(!s){
		GPIO_SetBits(GPIOE, GPIO_Pin_6);	
	}else{
		GPIO_ResetBits(GPIOE, GPIO_Pin_6);	
	}
}
