/**
@note 该文件主要描述外部中断服务	  
*/
#include "bsp_Interrupt.h"

InterruptInfo mdInterruptInfo;

/*中断IO配置*/
GpioInfo InterruptIoInfo[INTERRUPT_MAX_NUM] = 
{
	{InterruptChannel2Port,InterruptChannel2Pin},
	{InterruptChannel2Port,InterruptChannel2Pin},
	{InterruptChannel2Port,InterruptChannel2Pin},
	{InterruptChannel2Port,InterruptChannel2Pin}
};

uint32_t InterruptInitLineCfg[INTERRUPT_MAX_NUM] =
{
	InterruptChannel2Line,
	InterruptChannel2Line,
	InterruptChannel2Line,
	InterruptChannel2Line
};

uint8_t InterruptPinSourcecfg[INTERRUPT_MAX_NUM] = 
{
	InterruptChannel2PinSource,
	InterruptChannel2PinSource,
	InterruptChannel2PinSource,
	InterruptChannel2PinSource
};

uint8_t InterruptPortSourceccfg[INTERRUPT_MAX_NUM] =
{
	InterruptChannel2PortSource,
	InterruptChannel2PortSource,
	InterruptChannel2PortSource,
	InterruptChannel2PortSource
};
uint8_t InterruptIqrcfg[INTERRUPT_MAX_NUM] = 
{
	InterruptChannel2Irq,
	InterruptChannel2Irq,
	InterruptChannel2Irq,
	InterruptChannel2Irq
};

/*中断设置*/
static void GPIO_NVIC_InterruptInit(uint8_t ucChannel,uint8_t PreemptionPriority,
																	  uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = mdInterruptInfo.sInterruptInfo[ucChannel].ucInterruptIrq; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;       
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
  NVIC_Init(&NVIC_InitStructure);
	
}
/*外部中断初始化*/
static void GPIO_EXTI_InterruptInit(uint8_t ucChannel)
{
	 EXTI_InitTypeDef EXTI_InitStructure;
  /*清除中断线*/
	EXTI_ClearITPendingBit(mdInterruptInfo.sInterruptInfo[ucChannel].ullInterruptLine);
	
	/*选择中断IO*/
  GPIO_EXTILineConfig(mdInterruptInfo.sInterruptInfo[ucChannel].ucInterruptPortSource, 
	mdInterruptInfo.sInterruptInfo[ucChannel].ucInterruptPinSource);

  EXTI_InitStructure.EXTI_Line = mdInterruptInfo.sInterruptInfo[ucChannel].ullInterruptLine;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          
  EXTI_Init(&EXTI_InitStructure);
	
}

/*外部中断初始化，需要告知中断线和优先级等信息*/
void InterruptInit(uint8_t ucChannel,InterruptCallFunc func,uint8_t ucMastPriority,uint8_t ucSubPriority)
{
	uint8_t ucLoop = 0;
	static uint8_t ucParmFlag = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*中断IO信息参数配置*/
	for(ucLoop = 0;ucLoop < INTERRUPT_MAX_NUM && 
			ucParmFlag == 0;ucLoop++)
	{
		mdInterruptInfo.sInterruptInfo[ucLoop].InterruptIo.GPIO_Pin =
		InterruptIoInfo[ucLoop].GPIO_Pin;
		
		mdInterruptInfo.sInterruptInfo[ucLoop].InterruptIo.GPIO_Port =
		InterruptIoInfo[ucLoop].GPIO_Port;
		
		
		mdInterruptInfo.sInterruptInfo[ucLoop].ullInterruptLine = 
						InterruptInitLineCfg[ucLoop];/*中断线配置*/
		
		mdInterruptInfo.sInterruptInfo[ucLoop].ucInterruptPinSource = 
						InterruptPinSourcecfg[ucLoop];/*中断源配置*/
		
		mdInterruptInfo.sInterruptInfo[ucLoop].ucInterruptPortSource = 
						InterruptPortSourceccfg[ucLoop];/*中断源配置*/
						
		mdInterruptInfo.sInterruptInfo[ucLoop].ucInterruptIrq = 
						InterruptIqrcfg[ucLoop];/*中断源配置*/
	}
	ucParmFlag = 1;/*只初始化一次*/
	
	GPIO_InitStructure.GPIO_Pin = 
	mdInterruptInfo.sInterruptInfo[ucChannel].InterruptIo.GPIO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;/*浮空输入*/
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(mdInterruptInfo.sInterruptInfo[ucChannel].InterruptIo.GPIO_Port,
							&GPIO_InitStructure);	
	
	mdInterruptInfo.sInterruptInfo[ucChannel].func = func;
	
	GPIO_EXTI_InterruptInit(ucChannel);/*设置该通道的外部中断*/
	GPIO_NVIC_InterruptInit(ucChannel,ucMastPriority,
													ucSubPriority);
}
/*中断*/
void EXTI0_IRQHandler(void)
{
	/*do thing*/
	/*中断计数器计数*/
	if(mdInterruptInfo.sInterruptInfo[0].ullInterruptCount < 4294967295)
	{
	//	mdInterruptInfo.sInterruptInfo[0].ullInterruptCount++;
	}
	else
	{
		//mdInterruptInfo.sInterruptInfo[0].ucInterruptFullFlg = 1;
		//mdInterruptInfo.sInterruptInfo[0].ullInterruptCount = 0;
	}
	if(mdInterruptInfo.sInterruptInfo[0].func != NULL)
	{
		mdInterruptInfo.sInterruptInfo[0].func(NULL,NULL);/*回掉函数*/
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0);
}
/*获取当前中断个数*/
uint32_t InterruptGetCenterCount(InterruptNodeInf *InterruptNode)
{
	return InterruptNode->ullInterruptCount;
}
/*清除中断计数*/
void IneterruptCleanCount(InterruptNodeInf *InterruptNode)
{
	InterruptNode->ullInterruptCount = 0;
	InterruptNode->ucInterruptFullFlg = 0;
}
void EXTI1_IRQHandler(void)
{
	/*do thing*/
	/*中断计数器计数*/
	OSIntEnter();    //通知UCOS进入中断
	if(mdInterruptInfo.sInterruptInfo[1].ullInterruptCount < 4294967295)
	{
		mdInterruptInfo.sInterruptInfo[1].ullInterruptCount++;
	}
	else
	{
		mdInterruptInfo.sInterruptInfo[1].ucInterruptFullFlg = 1;
		mdInterruptInfo.sInterruptInfo[1].ullInterruptCount = 0;
	}
	if(mdInterruptInfo.sInterruptInfo[1].func != NULL)
	{
		mdInterruptInfo.sInterruptInfo[1].func(NULL,NULL);/*回掉函数*/
	}
	
	EXTI_ClearITPendingBit(EXTI_Line1);
	OSIntExit();
}

void EXTI2_IRQHandler(void)
{
	/*do thing*/
	/*中断计数器计数*/
	OSIntEnter();    //通知UCOS进入中断
	EXTI_ClearITPendingBit(EXTI_Line2);
	if(mdInterruptInfo.sInterruptInfo[0].ullInterruptCount < 4294967295)
	{
		mdInterruptInfo.sInterruptInfo[0].ullInterruptCount++;
		
	}
	else
	{
		mdInterruptInfo.sInterruptInfo[0].ucInterruptFullFlg = 1;
		mdInterruptInfo.sInterruptInfo[0].ullInterruptCount = 0;
	}
	OSIntExit();
}

void EXTI3_IRQHandler(void)
{
	/*do thing*/
	/*中断计数器计数*/
	OSIntEnter();    //通知UCOS进入中断
	if(mdInterruptInfo.sInterruptInfo[3].ullInterruptCount < 4294967295)
	{
		mdInterruptInfo.sInterruptInfo[3].ullInterruptCount++;
	}
	else
	{
		mdInterruptInfo.sInterruptInfo[3].ucInterruptFullFlg = 1;
		mdInterruptInfo.sInterruptInfo[3].ullInterruptCount = 0;
	}
	if(mdInterruptInfo.sInterruptInfo[3].func != NULL)
	{
		mdInterruptInfo.sInterruptInfo[3].func(NULL,NULL);/*回掉函数*/
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);
	OSIntExit();
}
