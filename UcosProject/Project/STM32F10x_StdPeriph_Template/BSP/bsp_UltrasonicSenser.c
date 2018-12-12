#include  "bsp_UltrasonicSenser.h"

volatile UltrasonicSenserInfo mdUltrasonicSenserInfo;
float cm = 0.0;
GpioInfo UltrasonicSenserGpioCfg[2] = 
{
	{UltrasonicSenserTxdPort,UltrasonicSenserTxd},	
	{UltrasonicSenserRxdPort,UltrasonicSenserRxd}
};

/*****************************************************************************
 * 函 数 名  : UltrasonicSenserTask
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月21日
 * 函数功能  : 超声波传感器1ms定时任务，没1ms进入一次该函数
 * 输入参数  : uint16_t *arg1  输入参数1
               uint16_t *arg2  输入参数2
 * 输出参数  : 无
 * 返 回 值  : uint16_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint16_t UltrasonicSenserTask(uint16_t *arg1,uint16_t *arg2)
{
 	uint16_t SenerId = 0;
	
 	if(arg1 != NULL)
 	{
		SenerId  = *arg1;
 	}
	mdUltrasonicSenserInfo.SenserNode[0].uliTimerMsCount++;
}

/*****************************************************************************
 * 函 数 名  : GetUltrasonicSenserEchoTime
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月21日
 * 函数功能  : 获取指定传感器的回复时间，获取完了需要清除
 * 输入参数  : uint8_t SenserId  传感器ID
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint32_t GetUltrasonicSenserEchoTime(uint8_t SenserId)
{
	uint32_t SenserEchoTime = 0;

	SenserEchoTime = mdUltrasonicSenserInfo.SenserNode[SenserId].uliTimerMsCount * 1000;/*us*/
	
	SenserEchoTime += TIM_GetCounter(TIM2);
	
	TIM2->CNT=0;
	
	mdUltrasonicSenserInfo.SenserNode[SenserId].uliTimerMsCount = 0;
	mdUltrasonicSenserInfo.SenserNode[SenserId].uliTimerBeginUsCount = 0;
	return SenserEchoTime;/*返回当前的值*/
}

void UltrasonicSenserDelay_Us(uint16_t time)  //延时函数
{ 
    uint16_t i,j;
    for(i=0;i<time;i++)
          for(j=0;j<9;j++);
}

void UltrasonicSenserTasks(void *p_arg)
{
	OS_ERR     err;
	float    fCm = 0;
	float    fCemterCm = 0;
	uint32_t ulTimes = 0;
	uint8_t  ucCount = 0;
	while(1)
	{
		ucCount = 0;
		while(ucCount < 10)
		{
			
			/*发送高电平*/
			GPIO_SetBits(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Port,
			mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Pin);/*发送高电平*/
			
			UltrasonicSenserDelay_Us(20);/*等待20us*/
			
			GPIO_ResetBits(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Port,
			mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Pin);/*发送低电平*/
			
			while(!GPIO_ReadInputDataBit(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Port,
				mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Pin));
			
			OpenCheckSenserEchoTimer(0);/*开始计数*/
			
			while(GPIO_ReadInputDataBit(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Port,
				mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Pin));
			
			ulTimes = GetUltrasonicSenserEchoTime(0);
			if(ulTimes <= 130)
			{
				ulTimes = 120;
			}
			fCemterCm = ((float)ulTimes / 58.0);
			if(fCemterCm > UltrasonicSenserMaxCm)
			{
				fCemterCm = UltrasonicSenserMaxCm;
			}
			fCm += fCemterCm;
			CloseCheckSenserEchoTimer(0);
			ucCount+=1;
			OSTimeDly(  (OS_TICK    )50, 
	                    (OS_OPT     )OS_OPT_TIME_DLY, 
	                    (OS_ERR     *)&err);/*1ms*/
		}
		fCm /= 10.0;
		//OSSemPend(&mdUltrasonicSenserInfo.Sem,OS_OPT_PEND_BLOCKING,1000,NULL,&err);/*等待信号量*/
		
		mdUltrasonicSenserInfo.SenserNode[0].fCenterCm = fCm;
		if(mdUltrasonicSenserInfo.SenserNode[0].fOldCm != fCm)
		{
			mdUltrasonicSenserInfo.SenserNode[0].fOldCm = mdUltrasonicSenserInfo.SenserNode[0].fCenterCm;
		}
		cm = fCm; 
		//OSSemPost(&mdUltrasonicSenserInfo.Sem,OS_OPT_POST_ALL,&err);/*释放信号量*/
		fCm = 0;
		OSTimeDly(  (OS_TICK    )500, 
	                    (OS_OPT     )OS_OPT_TIME_DLY, 
	                    (OS_ERR     *)&err);/*1ms*/
		
	}
}
/*获取水面距离传感器的高度*/
float GetCenterSenserCm(uint8_t SenserId)
{
	OS_ERR     err;
	float CenterCm = 0.0;
	//OSSemPend(&mdUltrasonicSenserInfo.Sem,OS_OPT_PEND_BLOCKING,1000,NULL,&err);/*等待信号量*/
	
	CenterCm = cm;
	//OSSemPost(&mdUltrasonicSenserInfo.Sem,OS_OPT_POST_ALL,&err);/*释放信号量*/
	return CenterCm;
}
void UltrasonicSenserInit()
{
	uint8_t ucLoop = 0;
	OS_ERR err;
	GPIO_InitTypeDef GPIO_InitStructure;
	//OSSemCreate(&mdUltrasonicSenserInfo.Sem, "SenserSem", 1, &err);/*信号量创建*/
	//OSSemPost(&mdUltrasonicSenserInfo.Sem,OS_OPT_POST_ALL,&err);/*释放信号量*/
	 
	TimerAdvanceInit(TIM2,1000,72,0,0,UltrasonicSenserTask,1,0);/*1ms一个中断*/
	//for(ucLoop = 0;ucLoop < 2;ucLoop++)
	{
		mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Port =  
		UltrasonicSenserGpioCfg[0].GPIO_Port;
		
		mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Pin =  
		UltrasonicSenserGpioCfg[0].GPIO_Pin;

		mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Port =  
		UltrasonicSenserGpioCfg[1].GPIO_Port;
		
		mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Pin =  
		UltrasonicSenserGpioCfg[1].GPIO_Pin;
		/*txd脚输出*/
		GPIO_InitStructure.GPIO_Pin = mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Port,&GPIO_InitStructure);
		GPIO_ResetBits(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Port,
					mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserTx.GPIO_Pin);

		/*rxd*/
		GPIO_InitStructure.GPIO_Pin = mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Pin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Port,&GPIO_InitStructure);
		GPIO_ResetBits(mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Port,
					mdUltrasonicSenserInfo.SenserNode[0].UltrasonicSenserRx.GPIO_Pin);
		
		mdUltrasonicSenserInfo.SenserNode[0].BindTimerNode = UltrasonicSenserBindTimer;
	}
	/*定时器绑定*/
	mdUltrasonicSenserInfo.SenserNode[0].BindInterruptNode = UltrasonicSenser1BindInter;
	mdUltrasonicSenserInfo.SenserNode[0].BindInterruptNode = UltrasonicSenser2BindInter;

	/*创建任务*/
	

	OSTaskCreate(   (OS_TCB     *)&mdUltrasonicSenserInfo.taskUltrasonicSenserTCB,
                    (CPU_CHAR   *)"UltrasonicSensers",
                    (OS_TASK_PTR)UltrasonicSenserTasks,
                    (void       *)0,
                    (OS_PRIO    )TASK2_PRIO,
                    (CPU_STK    *)&mdUltrasonicSenserInfo.taskUltrasonicSenserStk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);

	OSTimeDly(  (OS_TICK    )500, 
	                    (OS_OPT     )OS_OPT_TIME_DLY, 
	                    (OS_ERR     *)&err);/*100ms,让超声波传感器先运作起来*/
	
}
/*****************************************************************************
 * 函 数 名  : OpenCheckSenserEchoTimer
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月21日
 * 函数功能  : 开启超声波传感器的返回接受定时任务
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void OpenCheckSenserEchoTimer(uint8_t SenserId)
{
	TIM_SetCounter(TIM2,0);
	
	mdUltrasonicSenserInfo.SenserNode[SenserId].uliTimerMsCount = 0;

	TIM_Cmd(TIM2,ENABLE);
}
/*****************************************************************************
 * 函 数 名  : CloseCheckSenserEchoTimer
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月21日
 * 函数功能  : 删除指定任务
 * 输入参数  : uint8_t SenserId  传感器id
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void CloseCheckSenserEchoTimer(uint8_t SenserId)
{
	TIM_Cmd(TIM2,ENABLE);
}