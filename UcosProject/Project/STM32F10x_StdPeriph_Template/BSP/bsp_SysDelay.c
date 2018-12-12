#include "bsp_SysDelay.h"

//SysTick_s mdSysTick;

static __IO uint32_t TimingDelay=0;

void SysTick_Init(uint32_t a)
{
	if (SysTick_Config(SystemCoreClock / 1000000)*a)
	{ 
		/* Capture error */ 
		while (1);
	}
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void Delay_us(__IO u32 nTime)
{ 
  TimingDelay = nTime;	

	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0); 
}
void SysDelayMs(__IO u32 nTime)
{	 		  	  
	TimingDelay = nTime * 100;//???????
  //SysTick_Init(1000);  //1ms
  SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

  while(TimingDelay != 0);   
} 
void TimingDelay_Decrement(void)
{
		
    if(TimingDelay > 0)
		{
			TimingDelay--;
		}
		//GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}
void SysTick_Handler(void) 
{
	OS_ERR err;

	OSIntEnter();    //¨ª¡§?aUCOS??¨¨??D??
    TimingDelay_Decrement();	
	OSIntExit();
}

