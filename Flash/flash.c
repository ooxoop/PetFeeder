#include "flash.h"
#include "stm32f10x.h"
#include "spi_flash.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* ��ȡ�������ĳ��� */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  sFLASH_ID              0xEF4015	 //W25Q16

/* ���ͻ�������ʼ�� */
uint8_t Tx_Buffer[128] ;
uint8_t Rx_Buffer[BufferSize];
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

/* �������� */
uint8_t* flash_strcpy(uint8_t*, uint8_t*);
uint8_t* flash_strext(uint8_t*);
void flash_update(uint8_t*, uint8_t*);

//���ڴ�����ݹؼ���
uint8_t strkey[16];
//���ڴ����������
char strdata[128];
//���ڴ�ſͻ�ID
char pId[16] = "id,";

/* ��Flash�л�ȡ���� */
uint8_t* getFlashData(void){
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID();
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID){
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n[Flash]>>>��ȡ>>>%s\r\n", Rx_Buffer);
	}else{
		printf("\r\n[Flash]��ȡ���� W25Q16 ID!\r\n");
	}
	SPI_Flash_PowerDown();  
	return Rx_Buffer;
}

/* ������д�뵽Flash�� */
void setFlashData(uint8_t* data){
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID();
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID){
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	
		flash_strcpy(Tx_Buffer, Rx_Buffer);
		flash_strcpy(Tx_Buffer, data);
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\n[Flash]<<<д��<<<%s\r\n", Tx_Buffer);
	}else{
		printf("\r\n[Flash]��ȡ���� W25Q16 ID!\r\n");
	}
	SPI_Flash_PowerDown();  
}

/* ��Flash�л�ȡ�ͻ�ID */
char* getClientID(void){
	char data[BufferSize];
	char *p;
	int i;
	for(i=0;i<strlen((char *)Rx_Buffer);i++){
		if(Rx_Buffer[i] == '\0')
			break;
		data[i] = Rx_Buffer[i];
	}
	p = strtok(data, ",");
	p = strtok(NULL, ",");
	printf("\r\n[Flash]��ȡ�ͻ�IDΪ:%s \r\n", p);
	for(i=0; i<strlen(p); i++){
		pId[i+3] = p[i];
	}
	return pId;
}

/* �ַ���ƴ�� */
uint8_t* flash_strcpy(uint8_t* str1, uint8_t* str2){
	int len1, len2, i;
	char *p;
	p = strstr((char *)str1, (char *)flash_strext(str2));

	if(p){
		printf("\r\n[Flash]�����Ѵ���(%s) ", p);
		flash_update((uint8_t*)p, str2);
		//p = p + strlen((char *)strkey) + 1;
		//printf("��������Ϊ(%s)\r\n", flash_strext((uint8_t*)p));		
		return str1;
	}
	
	len1 = strlen((char *)str1);
	len2 = strlen((char *)str2);
	for(i=0; i<len2; i++){
		str1[len1+i] = str2[i];
	}
	return str1;
}

/* ��ȡ���ݹؼ��� */
uint8_t* flash_strext(uint8_t* str){
	int i;
	memset(strkey, 0, sizeof(strkey));
	for(i=0; i<strlen((char *)str); i++){
		if(str[i] == ','){
			break;
		}
		strkey[i] = str[i];
	}
	return strkey;
}

/* ������������ */
void flash_update(uint8_t* str1, uint8_t* str2){
	int len, i;
	
	len = strlen((char *)str2);
	for(i=0; i<len; i++){
		str1[i] = str2[i];
	}
	 
	printf("\r\n[Flash]�����Ѹ���(%s) ", str1);
}

/* ��flash�в���ָ���������� */
char* flash_getData(char* key){
	char *p;
	int i=0;
	
	p = strstr((char *)Rx_Buffer, key);
	if(p && (p + strlen(key))[0] == ','){
		p = p + strlen(key) + 1;
		while(p[i] != ','){
			strdata[i] = p[i];
			i++;
		}
		printf("\r\n[Flash]����(%s)����->(%s) \r\n", key, strdata);
		return strdata;
	}else{
		printf("\r\n[Flash]����(%s)������ \r\n", key);
		return NULL;
	}
	
}

int isFirstUse(void){
	char *ifu; 
		
	ifu = flash_getData("init");
	if(strstr(ifu, "1"))
		return 1;
	else if(strstr(ifu, "0"))
		return 0;
	
	return 2;
}
