#ifndef __DEVICE_H
#define __DEVICE_H

#include "stm32f10x.h"

extern int Motor_Status;

void motor_init(void);
void motor_status(int);

#endif
