#include "flash.h"
#include "stm32f10x.h"
#include "spi_flash.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  sFLASH_ID              0xEF4015	 //W25Q16

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[128] ;
uint8_t Rx_Buffer[BufferSize];
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

/* 函数声明 */
uint8_t* flash_strcpy(uint8_t*, uint8_t*);
uint8_t* flash_strext(uint8_t*);
void flash_update(uint8_t*, uint8_t*);

//用于存放数据关键词
uint8_t strkey[16];
//用于存放数据内容
char strdata[128];
//用于存放客户ID
char pId[16] = "id,";

/* 从Flash中获取数据 */
uint8_t* getFlashData(void){
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID();
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID){
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n[Flash]>>>读取>>>%s\r\n", Rx_Buffer);
	}else{
		printf("\r\n[Flash]获取不到 W25Q16 ID!\r\n");
	}
	SPI_Flash_PowerDown();  
	return Rx_Buffer;
}

/* 将数据写入到Flash中 */
void setFlashData(uint8_t* data){
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID();
	FlashID = SPI_FLASH_ReadID();
	if (FlashID == sFLASH_ID){
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	
		flash_strcpy(Tx_Buffer, Rx_Buffer);
		flash_strcpy(Tx_Buffer, data);
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\n[Flash]<<<写入<<<%s\r\n", Tx_Buffer);
	}else{
		printf("\r\n[Flash]获取不到 W25Q16 ID!\r\n");
	}
	SPI_Flash_PowerDown();  
}

/* 从Flash中获取客户ID */
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
	printf("\r\n[Flash]读取客户ID为:%s \r\n", p);
	for(i=0; i<strlen(p); i++){
		pId[i+3] = p[i];
	}
	return pId;
}

/* 字符串拼接 */
uint8_t* flash_strcpy(uint8_t* str1, uint8_t* str2){
	int len1, len2, i;
	char *p;
	p = strstr((char *)str1, (char *)flash_strext(str2));

	if(p){
		printf("\r\n[Flash]数据已存在(%s) ", p);
		flash_update((uint8_t*)p, str2);
		//p = p + strlen((char *)strkey) + 1;
		//printf("数据内容为(%s)\r\n", flash_strext((uint8_t*)p));		
		return str1;
	}
	
	len1 = strlen((char *)str1);
	len2 = strlen((char *)str2);
	for(i=0; i<len2; i++){
		str1[len1+i] = str2[i];
	}
	return str1;
}

/* 提取数据关键词 */
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

/* 更新数据内容 */
void flash_update(uint8_t* str1, uint8_t* str2){
	int len, i;
	
	len = strlen((char *)str2);
	for(i=0; i<len; i++){
		str1[i] = str2[i];
	}
	 
	printf("\r\n[Flash]数据已更新(%s) ", str1);
}

/* 从flash中查找指定数据内容 */
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
		printf("\r\n[Flash]数据(%s)存在->(%s) \r\n", key, strdata);
		return strdata;
	}else{
		printf("\r\n[Flash]数据(%s)不存在 \r\n", key);
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
