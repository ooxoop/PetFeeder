#ifndef __NETWORK_H
#define __NETWORK_H

#include "stm32f10x.h"

void net_linkToServer(int);
void net_bind(void);
void net_receive(char *);
void net_status(void);



#endif

