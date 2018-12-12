/***********************************************************************************
 * 文 件 名   : Bsp_led.h
 * 负 责 人   : Gavin
 * 创建日期   : 2017年4月1日
 * 文件描述   : 该文件用于描述LED指示灯的宏定义
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "BspConfig.h"

#define LED_NUM 4

#define LED1_PROT GPIOA
#define LED2_PROT GPIOA
#define LED3_PROT GPIOA
#define LED4_PROT GPIOA

#define LED1_Pin  GPIO_Pin_8
#define LED2_Pin 	GPIO_Pin_11
#define LED3_Pin  GPIO_Pin_12
#define LED4_Pin 	GPIO_Pin_8

typedef enum
{
	LED1 = 1,
	LED2,
	LED3,
	LED4
}LED;


typedef struct tagLedInfo_s
{
	uint16_t GPIO_Pin;				/*led所处引脚*/
	GPIO_TypeDef* GPIO_Port;	/*在那个端口上*/
	uint8_t UseFlage;					/*如果使用了这个就写1*/
	
}LedInfo;

typedef struct tagLedControl_s
{
	LedInfo LedInfo[LED_NUM];					/*系统共计控制4个LED灯*/
}LedControl;


void SysLedInit(void);

/*开灯操作*/
void LedOpen(uint8_t bits);
void LedClose(uint8_t bits);
#endif
