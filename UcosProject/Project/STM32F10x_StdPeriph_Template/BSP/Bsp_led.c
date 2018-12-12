#include "Bsp_led.h"
#include "stdio.h"
#include "string.h"

LedControl mdLed;

void EnableLedPort(uint8_t bits)
{
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	if(mdLed.LedInfo[bits].UseFlage == 1)
	{
		GPIO_Init_Structure.GPIO_Pin = mdLed.LedInfo[bits].GPIO_Pin; 
		GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(mdLed.LedInfo[bits].GPIO_Port,&GPIO_Init_Structure); /*led1*/
	}
}
/*****************************************************************************
 * 函 数 名  : SysLedParmInit
 * 负 责 人  : Gavin
 * 创建日期  : 2017年4月1日
 * 函数功能  : 系统LED灯参数初始化，主要描述LED指示灯的硬件连接关系
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SysLedParmInit()
{
	/*ledok*/
	mdLed.LedInfo[0].GPIO_Port = LED1_PROT;
	mdLed.LedInfo[0].GPIO_Pin = LED1_Pin;
	mdLed.LedInfo[0].UseFlage = 1;
	
	mdLed.LedInfo[1].GPIO_Port = LED2_PROT;
	mdLed.LedInfo[1].GPIO_Pin = LED2_Pin;
	mdLed.LedInfo[1].UseFlage = 1;
	
	mdLed.LedInfo[2].GPIO_Port = LED3_PROT;
	mdLed.LedInfo[2].GPIO_Pin = LED3_Pin;
	mdLed.LedInfo[2].UseFlage = 1;

}
/*****************************************************************************
 * 函 数 名  : SysLedInit
 * 负 责 人  : Gavin
 * 创建日期  : 2017年4月1日
 * 函数功能  : 系统指示灯初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SysLedInit()
{
	uint8_t ucLoop = 0;
	memset(&mdLed,0,sizeof(mdLed));
	
	SysLedParmInit();
	
	for(ucLoop = 0; ucLoop < 3; ucLoop++)
	{
		EnableLedPort(ucLoop);
		LedClose(ucLoop + 1);
	}
}
/*****************************************************************************
 * 函 数 名  : LedIoControl
 * 负 责 人  : Gavin
 * 创建日期  : 2017年4月1日
 * 函数功能  : 该函数用于控制LED灯
 * 输入参数  : uint8_t bits   LED灯标示
               uint8_t flage  取值范围（0-1）0代表关闭，1代表开启
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void LedIoControl(uint8_t bits,uint8_t flage)
{
	if(mdLed.LedInfo[bits].UseFlage == 1)
	{
		if(flage)
		{
			GPIO_SetBits(mdLed.LedInfo[bits].GPIO_Port,
									mdLed.LedInfo[bits].GPIO_Pin);
		}
		else
		{
			GPIO_ResetBits(mdLed.LedInfo[bits].GPIO_Port,
									mdLed.LedInfo[bits].GPIO_Pin);
		}
	}
}
/*****************************************************************************
 * 函 数 名  : LedOpen
 * 负 责 人  : Gavin
 * 创建日期  : 2017年4月1日
 * 函数功能  : LED指示灯开启操作
 * 输入参数  : uint8_t bits  LED灯
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void LedOpen(uint8_t bits)
{
	LedIoControl(bits -1,0);
}

/*****************************************************************************
 * 函 数 名  : LedClose
 * 负 责 人  : Gavin
 * 创建日期  : 2017年4月1日
 * 函数功能  : LED指示灯关闭操作
 * 输入参数  : uint8_t bits  LED指示灯
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void LedClose(uint8_t bits)
{
	LedIoControl(bits -1,1);
}
