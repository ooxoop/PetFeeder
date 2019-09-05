#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"

uint8_t* getFlashData(void);
void setFlashData(uint8_t*);
char* getClientID(void);
uint8_t* flash_strcpy(uint8_t*, uint8_t*);
char* flash_getData(char*);
int isFirstUse(void);



#endif
