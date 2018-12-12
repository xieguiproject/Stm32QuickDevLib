/****************************************Copyright (c)**************************************************
** Modified by:       
** Modified date:      2012-05-20 
** Version:            v3.0
** Descriptions:       修改用于STM32F10x   
**
**------------------------------------------------------------------------------------------------------
** Modified by: 		
** Modified date:	   
** Version: 		   
** Descriptions:		
**
********************************************************************************************************/

#ifndef _bsp_h_
#define _bsp_h_

#include "os.h" 
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Bsp_led.h"
#include "BspConfig.h"
#include "Bsp_usart.h"
#include "Bsp_Esp8266.h"
#include "bsp_Calendar.h"
#include "time.h"
#include "bsp_Timer.h"
#include "bsp_Interrupt.h"
#include "bsp_UltrasonicSenser.h"
#include "bsp_Flash.h"
#include "bsp_OnenetServer.h"
#include "bsp_Lcd1602.h"
/********************************/
/*       系统配置函数           */
/********************************/


void IWDG_Init(void);
void SysTickInit(void);
void BspInit(void);
void led_init(void);


#define TP_CS()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define TP_DCS() GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define WaitTPReady() while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0)

void SPI_Config(void);
void SpiDelay(unsigned int DelayCnt);
unsigned char SPI_WriteByte(unsigned char data) ;
unsigned short TPReadX(void);
unsigned short TPReadY(void);

#define LOG_RUN_PRINTF 			0
#define LOG_DEBUG_PRINTF 		1

#endif

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/


