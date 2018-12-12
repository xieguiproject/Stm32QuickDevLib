#include "bsp_Timer.h"
#include "string.h"
TimerInfo mdTimer;


void TimerNvicConfig(uint8_t TimerIrqChannel,uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);   // 抢占式优先级别 
	
	NVIC_InitStructure.NVIC_IRQChannel = TimerIrqChannel;//指定中断源
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;// 指定响应优先级别1
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

/*****************************************************************************
 * 函 数 名  : TimerInit
 * 负 责 人  : xiegui
 * 创建日期  : 2017年11月28日
 *@brief 函数功能  :  定时器初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void TimerAdvanceInit(TIM_TypeDef* Timer,uint16_t TimerPeriod,uint16_t TimerPrescaler,
				uint16_t TimerMode,uint16_t TimerClockDivision,
				TimerCallFunc callFunc,uint8_t ucFlage,uint8_t ucPwmEnable)/*TIM_CounterMode_Up*/
{
     TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	 
	 
	 TIM_TimeBaseStructure.TIM_Period = TimerPeriod - 1;/*定时器计数值*/
	 
	 TIM_TimeBaseStructure.TIM_Prescaler = TimerPrescaler - 1;
	 /*定时器预先分频值，时钟是72MHZ，如果需要定时1ms,可以采用下面的值进行设置，但最好不要
	 设置1MS，因为中断太频繁，造成任务一直没有运行时间
	 72000000/36000 = 2000HZ= 0.5mS,如果TimerPeriod == 2000，则代表定时1S
	 如果要定时10ms,TimerPeriod = 200
	 定时1us的方法：
	 72000000/72 = 1000000Hz = 1Mhz   1us*/
	 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	 
	 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	 switch((uint32_t)Timer)
	 {
		case (uint32_t)TIM2:
			{
				TIM_InternalClockConfig(TIM2);
				if(ucFlage)/*强制初始化*/
				{
					mdTimer.Timer2.TimerUseSate =0;
				}
				if(!mdTimer.Timer2.TimerUseSate)
				{
					mdTimer.Timer2.TimerPeriod = TimerPeriod;
					mdTimer.Timer2.TimerPrescaler = TimerPrescaler;
					mdTimer.Timer2.TimerCounterMode = TimerMode;
					mdTimer.Timer2.IntBackFun = callFunc;
					mdTimer.Timer2.TimerUseSate = 1;
					mdTimer.Timer2.TimetBaseMs = (TimerPeriod / 2);
					mdTimer.Timer2.TimerInitOk = 1;
					TIM_DeInit(TIM2);
					TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
					if(!ucPwmEnable)
					{
						TIM_ClearFlag(TIM2,TIM_FLAG_Update);
						 TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
						TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
						//TIM_Cmd(TIM2, ENABLE);
						TimerNvicConfig(TIM2_IRQn,0);
					}
				}
			};break;
		case (uint32_t)TIM3:
			{
				TIM_InternalClockConfig(TIM3);
				if(ucFlage)/*强制初始化*/
				{
					mdTimer.Timer3.TimerUseSate =0;
				}
				if(!mdTimer.Timer3.TimerUseSate)
				{
					mdTimer.Timer3.TimerPeriod = TimerPeriod;
					mdTimer.Timer3.TimerPrescaler = TimerPeriod;
					mdTimer.Timer3.TimerCounterMode = TimerMode;
					mdTimer.Timer3.IntBackFun = callFunc;
					mdTimer.Timer3.TimerUseSate = 1;
					mdTimer.Timer3.TimetBaseMs = (TimerPeriod / 2);
					mdTimer.Timer3.TimerInitOk = 1;
					TIM_DeInit(TIM3);
					TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
					if(!ucPwmEnable)
					{
						TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
						 TIM_ClearITPendingBit(TIM3 , TIM_IT_Update);
						TIM_Cmd(TIM3, ENABLE);
						TimerNvicConfig(TIM3_IRQn,1);
					}
				}
			};break;
	 }
	  
}

/*****************************************************************************
 * 函 数 名  : TimerInit
 * 负 责 人  : xiegui
 * 创建日期  : 2017年11月30日
 * 函数功能  : 
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void  TimerInit(TIM_TypeDef* Timer,uint16_t uiMsDelay,TimerCallFunc callFunc,uint8_t ucFlage,uint8_t ucPwmEnable)
{
	/*定时范围0.1MS - 65536 * 0.5ms ---- 0.1ms - 32.768s*/
		//memset(&mdTimer,0,sizeof(TimerInfo));
    TimerAdvanceInit(Timer,uiMsDelay * 2,36000,0,0,callFunc,ucFlage,ucPwmEnable);
}
/*获取一个空闲的定时任务存储空间*/
int8_t TimerTaskGetFreeReg(TimerNodeInfo *TimerNode)
{
	int8_t ucLoop = 0;
	
	for(ucLoop = 0;ucLoop < MAX_TIMER_TASK;ucLoop++)
	{
		if(TimerNode->TimerTask[ucLoop].TimerNodeStatus == TIMER_TASK_BUF_FREE)
		{
			TimerNode->TimerTask[ucLoop].TimerNodeStatus = TIMER_TASK_BUF_USE;
			return ucLoop;
		}
	}
	return -1;
}

/*向定时任务组中添加一个定时任务
添加成功返回对应的Id，否则返回-1
，输入参数，1、要添加到的定时器，2、要添加的回掉函数、
						3、要添加的参数1，   4、要添加的参数2
						5、定时任务模式			 6、定时任务定时执行时间*/
int8_t TimerTaskAdd(TimerNodeInfo *TimerNode,TimerCallFunc hook,
										uint16_t arg1,uint16_t arg2,uint8_t TaskMode,
										uint32_t uiTime)
{
	int8_t TaskId = 0;
	/*获取当前定时器空闲的位置，*/
	TaskId = TimerTaskGetFreeReg(TimerNode);
	if(TaskId != -1)
	{
		TimerNode->TimerTask[TaskId].Func = hook;
		TimerNode->TimerTask[TaskId].arg1 = arg1;
		TimerNode->TimerTask[TaskId].arg2 = arg2;
		TimerNode->TimerTask[TaskId].TimerMode = TaskMode;
		TimerNode->TimerTask[TaskId].uiDelayMs = uiTime;
		TimerNode->TimerTask[TaskId].uiDelayCount = 0;
		return TaskId;
	}
	return -1;
}
/*定时任务删除，这里的删除机制很简单，只需要将任务的模式进行修改为只执行一次
即可，千万不要去操作其他，容易引起错误*/
int8_t TimerTaskDelete(TimerNodeInfo *TimerNode,int8_t TimerTaskId)
{
	TimerNode->TimerTask[TimerTaskId].TimerMode = TIMER_TASK_ONECE; /*下次执行完自动退出*/
}
/*****************************************************************************
 * 函 数 名  : TIM1_IRQHandler
 * 负 责 人  : xiegui
 * 创建日期  : 2017年11月28日
 * 函数功能  : 定时器1中断服务
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void TIM1_IRQHandler()
{
	
}
/*****************************************************************************
 * 函 数 名  : TIM3_IRQHandler
 * 负 责 人  : xiegui
 * 创建日期  : 2017年11月28日
 * 函数功能  : 定时器1中断服务
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 
*****************************************************************************/

void TIM2_IRQHandler()
{
		OS_ERR err;
	uint8_t ucLoop = 0;
	static uint16_t ucCount = 0;
	OSIntEnter();    //通知UCOS进入中断
	
	if(TIM_GetITStatus(TIM2 , TIM_IT_Update) == SET)
	{
	  TIM_ClearITPendingBit(TIM2 , TIM_IT_Update);
	  /*do user thing*/
	  if(mdTimer.Timer2.IntBackFun != NULL)
	  {
			mdTimer.Timer2.IntBackFun(NULL,NULL);
	  }
		for(ucLoop = 0;ucLoop < MAX_TIMER_TASK;ucLoop++)
		{
			if( (((++mdTimer.Timer2.TimerTask[ucLoop].uiDelayCount)) >= 
				mdTimer.Timer2.TimerTask[ucLoop].uiDelayMs) &&
				mdTimer.Timer2.TimerTask[ucLoop].TimerNodeStatus == TIMER_TASK_BUF_USE &&
				mdTimer.Timer2.TimerTask[ucLoop].Func != NULL)
			{
				/*do thing*/
				mdTimer.Timer2.TimerTask[ucLoop].uiDelayCount =0;
				mdTimer.Timer2.TimerTask[ucLoop].Func(&mdTimer.Timer2.TimerTask[ucLoop].arg1,
				&mdTimer.Timer2.TimerTask[ucLoop].arg2);
				if(mdTimer.Timer2.TimerTask[ucLoop].TimerMode == TIMER_TASK_ONECE)
				{
					//memset(&mdTimer.Timer2.TimerTask[0],0,sizeof(TimerCallFunInfo));
					mdTimer.Timer2.TimerTask[ucLoop].TimerNodeStatus  = TIMER_TASK_BUF_FREE;
				}
			}
		}
		/*定时任务*/
	}
	OSIntExit();
}

/*****************************************************************************
 * 函 数 名  : TIM3_IRQHandler
 * 负 责 人  : xiegui
 * 创建日期  : 2017年11月28日
 * 函数功能  : 定时器1中断服务
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/

void TIM3_IRQHandler()
{
	OS_ERR err;
	uint8_t ucLoop = 0;
	OSIntEnter();    //通知UCOS进入中断
	
	if(TIM_GetITStatus(TIM3 , TIM_IT_Update) == SET)
	{
	  TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
	  /*do user thing*/
	  if(mdTimer.Timer3.IntBackFun != NULL)
	  {
			mdTimer.Timer3.IntBackFun(NULL,NULL);
	  }
		for(ucLoop = 0;ucLoop < MAX_TIMER_TASK;ucLoop++)
		{
			
			if( ((mdTimer.Timer3.TimetBaseMs * (++mdTimer.Timer3.TimerTask[ucLoop].uiDelayCount)) >= 
				mdTimer.Timer3.TimerTask[ucLoop].uiDelayMs) &&
				mdTimer.Timer3.TimerTask[ucLoop].TimerNodeStatus == TIMER_TASK_BUF_USE &&
				mdTimer.Timer3.TimerTask[ucLoop].Func != NULL)
			{
				/*do thing*/
				mdTimer.Timer3.TimerTask[ucLoop].uiDelayCount =0;
				mdTimer.Timer3.TimerTask[ucLoop].Func(&mdTimer.Timer3.TimerTask[ucLoop].arg1,
				&mdTimer.Timer3.TimerTask[ucLoop].arg2);
				if(mdTimer.Timer3.TimerTask[ucLoop].TimerMode == TIMER_TASK_ONECE)
				{
					memset(&mdTimer.Timer3.TimerTask[ucLoop],0,sizeof(TimerCallFunInfo));
				}
			}
		}
		/*定时任务*/
	}
	OSIntExit();
}


