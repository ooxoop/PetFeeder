#include "network.h"
#include "stm32f10x.h"
#include "spi_flash.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "linked_list.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

#include "flash.h"
#include "led.h"
#include "tim2.h"



char recBuf[128];
char *recP;
int flag_link = 5;


/* ���ӷ����� */
void net_linkToServer(int ifu){
	int f = 0, s1 = 1, s2 = 1;
	
	macESP8266_CH_ENABLE();
	ESP8266_AT_Test ();
	

	if(ifu == isFirstUse()){
		printf("\r\n[Net]��һ��ʹ����Ϊ�������������ȵ�\r\n");
		while( !ESP8266_Net_Mode_Choose ( AP ) );
		ESP8266_Enable_MultipleId ( ENABLE );
		while( !ESP8266_BuildAP( "petmaid", "88888888", OPEN) )
			if(f++ == flag_link)
				break;
		f = 0;
		while( !ESP8266_StartOrShutServer( ENABLE, "8032", "3") );
	}else{
		printf("\r\n[Net]���ڳ��������ȵ�...\r\n");
		while( !ESP8266_Net_Mode_Choose ( STA ) );

		while( (s1 = !ESP8266_JoinAP( "stmstm", "66666666" )) == 1 )
			if(f++ == flag_link)
					break;
		f = 0;
		ESP8266_Enable_MultipleId ( DISABLE );
		while( (s2 = !ESP8266_Link_Server( enumTCP, "123.207.253.200", "8032", Single_ID_0 )) == 1)
			if(f++ == flag_link)
				break;
	}
	
	

	//while(!ESP8266_Link_Server(enumTCP, "123.207.253.200", "8032", Single_ID_0));
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
	//while(!ESP8266_SendString ( DISABLE, (char*)flashID, strlen ( (char*)flashID ), Single_ID_0)){}
		
	if(s1){
		printf("[Net]wifi����ʧ�ܣ�");
	}else if(s2){
		printf("[Net]����������ʧ�ܣ�");
	}else if(!s1 && !s2){
		printf("[Net]���������ӳɹ���");
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);	
	}else{
		printf("[Net]Error(%d),(%d)", s1, s2);
	}
}

/* ����������Ϣ */
void net_bind(void){
	int i;
	if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag ){
		//USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
		strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "IPD," ) ) {
			for(i=0; i<strlen(strEsp8266_Fram_Record .Data_RX_BUF); i++){
				recBuf[i] = strEsp8266_Fram_Record .Data_RX_BUF[i];
			}
			recP = strtok(recBuf, ":");
			recP = strtok(NULL, ":");
			recP[strlen(recP)-1] = '\0';
			printf ( "\r\n[Net]%s\r\n", recP );
			net_receive(recP);
			memset(recBuf,0,sizeof(recBuf));
			recP = NULL;
		}
		//printf ( "\r\n[msg]%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
		strEsp8266_Fram_Record .InfBit .FramLength = 0;
		strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;		
		USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
	}
}

/* ����������Ϣ��������� */
void net_receive(char* buf){
  char* p1 = "cmd,getplan";
	//printf ( "\r\n[judge]%d, %d\r\n", strcmp(buf, "connected"), strlen(buf));
	if(strcmp(buf, "connected") == 0){
		printf ( "\r\n[Net]���������ȡιʳ�ƻ�...\r\n" );
		while(!ESP8266_SendString ( DISABLE, p1, strlen ( p1 ), Single_ID_0)){}
	}else if(strstr(buf, "feedplan,")){
		printf ( "\r\n[Net]���ڱ���ιʳ�ƻ�...\r\n" );
		setFlashData((uint8_t *)buf);
	}else if(strstr(buf, "diet,")){
		setFlashData((uint8_t *)buf);
		list_free(pList);
		pList = list_saveDiet(flash_getData("diet"));
	}else if(strstr(buf, "feedNow,")){
		apk_flag = 1;
		printf("\r\n[Net]���յ�����ιʳָ�ιʳ����%s\r\n", (strstr(buf, "feedNow,")+8));
	}else if(strstr(buf, "ap_name,")){
		setFlashData((uint8_t *)buf);
	}else if(strstr(buf, "ap_pwd,")){
		setFlashData((uint8_t *)buf);
		setFlashData((uint8_t *)"init,1");
	}
}

/* �����������״̬ */
void net_status(void){
	if(ESP8266_Get_LinkStatus() == 4){
		printf("\r\n[Net]�������ʧȥ���ӣ��������½���ͨѶ...\r\n");
		while(!ESP8266_JoinAP( "10184G", "zangli82496" ));
		ESP8266_Enable_MultipleId ( DISABLE );
		while(!ESP8266_Link_Server( enumTCP, "123.207.253.200", "8032", Single_ID_0 ));
		printf("\r\n[Net]�������Ϸ�����\r\n");
	}else{
		printf("\r\n[Net]���ӻ�Ծ��\r\n");
	}
}

/*
void buf_repair(){
	int length = 0, i;
	for(i=0; i<128; i++){
		if(Rx_Buffer[i] == ';')
			break;		
		length++;
	}
	Rx_Buffer[length] = '\0';
}

*/



