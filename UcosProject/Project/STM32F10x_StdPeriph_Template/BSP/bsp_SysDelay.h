#ifndef BSP_SYSDELAY_H
#define BSP_SYSDELAY_H

#include "stm32f10x.h"
#include "os.h"
typedef struct tagSysTick_s
{
	uint32_t TimingDelay;/*??¨ºy?¡Â*/
}SysTick_s;
void SysDelayMs(__IO u32 nTime);
void SysTick_Init(uint32_t a);
void Delay_us(__IO u32 nTime);
extern SysTick_s mdSysTick;

#endif