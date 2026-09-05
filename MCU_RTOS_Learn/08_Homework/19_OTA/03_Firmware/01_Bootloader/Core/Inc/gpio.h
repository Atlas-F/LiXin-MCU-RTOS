/**
  ******************************************************************************
	WeAct ΢�д��� 
	>> ��׼�� GPIO
  ******************************************************************************
  */

#ifndef __gpio_H
#define __gpio_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "main.h"

void GPIO_Config(void);

void Key_IO_Init(void);
uint8_t KeyScan(void);
void LED_IO_Init(void);




#ifdef __cplusplus
}
#endif

#endif
