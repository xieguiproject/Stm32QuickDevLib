/***********************************************************************************
 * 文 件 名   : BspConfig.h
 * 负 责 人   : Gavin
 * 创建日期   : 2017年6月7日
 * 文件描述   : 该文件主要是用来配置硬件各个设备的详细参数,期望是用户只需要
                配置该文件就可以快速的完成驱动开发
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

#include "stm32f10x.h"

typedef struct tagGpioInfo
{
	GPIO_TypeDef*   GPIO_Port;
	uint16_t 		GPIO_Pin;		
}GpioInfo;


#define GPIOControl(Port,Pin,Flage) \
	if(Flage) GPIO_SetBits(Port,Pin);\
	else GPIO_ResetBits(Port,Pin);

#endif
