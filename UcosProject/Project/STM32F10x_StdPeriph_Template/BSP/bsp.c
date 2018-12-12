   /*
********************************************************************************
*                                  uC/OS-III
*                              ARM Cortex-M3 Port
*
* File      	: Main.C
* Version   	: V1.01
* By        	: 
*
* For       	: Stm32f10x
* Mode      	: 
* Toolchain 	: 
*             		RealView Microcontroller Development Kit (MDK)
*             		Keil uVision
* Description   : 采用STM32F10x芯片 的uC/OS-III平台上的软件开发模板
*
*					1，STM32F10x 3.5库
*					2，uC/OS-III v3.03
*				
* Date          : 
*******************************************************************************/

#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_spi.h"
#include "bsp.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//打开相应的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	//使用外部中断
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	//打开c端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	//打开c端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);	//打开c端口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);	//打开c端口的时?
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);	//打开c端口的时钟?
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //打开串口usart
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//端口复用功能
}

/*******************************************************************************
函 数 名：void IWDG_Init(void)
功能描述：看门狗初始化						
入口参数：							
返回参数：
创建时间: 2011.6.24
********************************************************************************/
void IWDG_Init(void)
{
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	IWDG_SetPrescaler( IWDG_Prescaler_64);	//最小
	IWDG_SetReload( 0x138);		//40KHz内部时钟 (1/40000 * 64 * 0x138 = 0.5s)
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Disable );
	IWDG_Enable();
	IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name :void SysTickInit(void)
* Description   :系统定时器时间配置
* Input         :
* Output        :
* Other         :时基为1ms
* Date          :2011.11.03  12:59:13
*******************************************************************************/
void SysTickInit(void)
{
	SysTick_Config(SystemCoreClock / 1000);			//uCOS时基1ms
}

/*******************************************************************************
* Function Name :void InterruptOrder(void)
* Description   :中断向量，优先级
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  11:50:05
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);//优先级设置  全为抢占式优先级
}


/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :系统时间戳 初始化
* Input         :
* Output        :
* Other         :
* Date          :2012.6.15  13:14:59
*******************************************************************************/
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
}
#endif


/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :读时间戳 计数值
* Input         :读到的计数值
* Output        :
* Other         :
* Date          :2012.6.15  13:14:59
*******************************************************************************/
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    return (SysTick->VAL);
}
#endif

/*******************************************************************************
* Function Name :void SystemConfig(void)
* Description   :系统初始化
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  13:14:59
*******************************************************************************/
void BspInit(void)
{
	SysTickInit();
	
	NVIC_Configuration();	//中断优先级设置
	GPIO_Configuration();	//端口初始化，所有端口关
	RelayInit();
	SysLedInit();
	
	USARTInit(USART1,115200,NULL);/*系统打印串口初始化*/
	mdUsart.USART1_s.USART_RecvQueue.QueueType = QueueArryType;
}



