#include "Bsp_usart.h"
#include "string.h"

USART_s mdUsart;	/*串口模块*/

void USARTGpioConfig(USARTInfo_s *UsartInfo)
{
	GPIO_InitTypeDef GPIO_InitStructure;		//声明一个结构体变量

	GPIO_InitStructure.GPIO_Pin = UsartInfo->GPIO_Pin_txd;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UsartInfo->GPIO_USART,&GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = UsartInfo->GPIO_Pin_rxd; 	//PA10---RXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //复用输入
	GPIO_Init(UsartInfo->GPIO_USART,&GPIO_InitStructure);		
} 

void USARTNvicConfig(uint8_t NVIC_IRQChannel,uint8_t MasterPro,uint8_t SlaPro)
{
		/*NVIC 的设置方法以及循序
		1、NVIC_IRQChannel;设置中断号
		2、NVIC_IRQChannelPreemptionPriority;设置先占优先级 
		3、NVIC_IRQChannelSubPriority;//设置从优先级 
		4、NVIC_IRQChannelCmd使能
	*/
	NVIC_InitTypeDef NVIC_Config;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_Config.NVIC_IRQChannel = NVIC_IRQChannel;  //选择中断源 
	NVIC_Config.NVIC_IRQChannelPreemptionPriority= MasterPro;  //抢占优先级0
	NVIC_Config.NVIC_IRQChannelSubPriority = SlaPro; //从优先级为0
	NVIC_Config.NVIC_IRQChannelCmd = ENABLE;//使能
	
	NVIC_Init (&NVIC_Config);	
}

void USARTConfig(USART_TypeDef* USARTx,USARTInfo_s *UsartInfo)
{
	USART_InitTypeDef USART_Init_Structure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	USART_DeInit(USARTx);
	
	USART_Init_Structure.USART_BaudRate = UsartInfo->USART_BaudRate;//波特率
	USART_Init_Structure.USART_WordLength = USART_WordLength_8b;
	USART_Init_Structure.USART_StopBits = USART_StopBits_1;	 //停止位位1
	USART_Init_Structure.USART_Parity = USART_Parity_No;	//偶校验
	USART_Init_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不是用硬件流控制
	USART_Init_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
	//USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	//USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
	//USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;

	USART_Init(USARTx,&USART_Init_Structure);
    
	//USART_ClockInit(USARTx,&USART_ClockInitStructure);

	USART_Cmd(USARTx,ENABLE);
	USART_ClearFlag(USARTx,USART_FLAG_TC);
	USART_ClearFlag(USARTx,USART_IT_RXNE);
	USART_ClearITPendingBit(USARTx,USART_IT_TC);
	USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE); //允许接收中断

}

#if 0
/*发送*/
	 if(USART_GetITStatus(USART1,USART_FLAG_TXE) != RESET)  
	 {
	 	/*出队成功*/
		if(DeQueue(&mdUsart.USART1_s.USART_SendQueue,&SendData))
		{
			USART_SendData(USART1,SendData);/*发送数据*/
		}
		else
		{
			USART_ITConfig(USART1, USART_FLAG_TXE, DISABLE);
		}
		
	 }
	 #endif
/*发送函数*/
void SendTask(void *p_arg)
{
	USARTInfo_s *UsartInfo = (USARTInfo_s *)p_arg;
	
	uint16_t SendData = 0;
	OS_ERR     err;
	while(1)
	{
		OSTimeDly(  (OS_TICK    )1, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		/*出队成功*/
					
		if(DeQueue(&UsartInfo->USART_SendQueue,&SendData))
		{
				USART_SendData(UsartInfo->USARTx,SendData);/*发送数据*/
		}
	}
}
/*创建一个发送任务，负责将队列中的数据取出来进行发送*/
void USART_SendTaskCreate(USARTInfo_s *Usart)
{
	OS_ERR 		err;
	
	OSTaskCreate(   (OS_TCB     *)&Usart->taskSendTCB,
                    (CPU_CHAR   *)Usart->UsartName,
                    (OS_TASK_PTR)SendTask,/*入口函数*/
                    (void       *)Usart,
                    (OS_PRIO    )TASK1_PRIO,
                    (CPU_STK    *)&Usart->taskSendStk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);

}
void USARTInit(USART_TypeDef* USARTx,uint32_t USART_BaudRate,
					USARTCallFunc CallBackFun)
{
	OS_ERR     err;
	switch((uint32_t)USARTx)
	{
		case (uint32_t)USART1:/*GPIOA USART1 RXD-GPIO9 TXD-GPIO10*/
				{
					mdUsart.USART1_s.GPIO_Pin_rxd = USART1_RXD;/*这个是不需要传参数的*/
					mdUsart.USART1_s.GPIO_Pin_txd = USART1_TXD;
					mdUsart.USART1_s.USART_BaudRate = USART_BaudRate;
					mdUsart.USART1_s.GPIO_USART = USART1_PORT; 
					mdUsart.USART1_s.USARTUserDel = CallBackFun;
					/*用户回掉函数注册*/
					
					USARTGpioConfig(&mdUsart.USART1_s);/*io口配置*/
					USARTNvicConfig(USART1_IRQn,7,0);
					USARTConfig(USART1,&mdUsart.USART1_s);
					/*发送信号量创建*/
					OSSemCreate(&mdUsart.USART1_s.USART_SendSem, "USART1_SendSem", 1, &err);
					OSSemCreate(&mdUsart.USART1_s.USART_RecvSem, "USART1_RecvSem", 1, &err);
                                    OSSemPost(&mdUsart.USART1_s.USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
                                    OSSemPost(&mdUsart.USART1_s.USART_RecvSem,OS_OPT_POST_ALL,&err);/*释放信*/
					strcpy(mdUsart.USART1_s.UsartName,"USART1");
					mdUsart.USART1_s.USARTx = USART1;
					USART_SendTaskCreate(&mdUsart.USART1_s);
					break;
				}
		case (uint32_t)USART2:
				{
					mdUsart.USART2_s.GPIO_Pin_rxd = USART2_RXD;
					mdUsart.USART2_s.GPIO_Pin_txd = USART2_TXD;
					mdUsart.USART2_s.USART_BaudRate = USART_BaudRate;
					mdUsart.USART2_s.GPIO_USART = USART2_PORT;
					mdUsart.USART2_s.USARTUserDel = CallBackFun;
					/*用户回掉函数注册*/
					
					USARTGpioConfig(&mdUsart.USART2_s);/*io口配置*/
					USARTNvicConfig(USART2_IRQn,0,0);
					USARTConfig(USART2,&mdUsart.USART2_s);
					OSSemCreate(&mdUsart.USART2_s.USART_SendSem, "USART2_SendSem", 1, &err);
					OSSemCreate(&mdUsart.USART2_s.USART_RecvSem, "USART2_RecvSem", 1, &err);
                                   OSSemPost(&mdUsart.USART2_s.USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
                                    OSSemPost(&mdUsart.USART2_s.USART_RecvSem,OS_OPT_POST_ALL,&err);/*释放信*/
					strcpy(mdUsart.USART2_s.UsartName,"USART2");
					mdUsart.USART2_s.USARTx = USART2;
					USART_SendTaskCreate(&mdUsart.USART2_s);
					break;
				}
#ifdef STM32F10X_HD

	  case (uint32_t)USART3:
				{
					mdUsart.USART3_s.GPIO_Pin_rxd = USART3_RXD;
					mdUsart.USART3_s.GPIO_Pin_txd = USART3_TXD;
					mdUsart.USART3_s.USART_BaudRate = USART_BaudRate;
					mdUsart.USART3_s.GPIO_USART = USART3_PORT;
					mdUsart.USART3_s.USARTUserDel = CallBackFun;
					/*用户回掉函数注册*/
					
					USARTGpioConfig(&mdUsart.USART3_s);/*io口配置*/
					USARTNvicConfig(USART3_IRQn,0,0);
					USARTConfig(USART3,&mdUsart.USART3_s);
					OSSemCreate(&mdUsart.USART3_s.USART_SendSem, "USART3_SendSem", 1, &err);
					OSSemCreate(&mdUsart.USART3_s.USART_RecvSem, "USART3_RecvSem", 1, &err);
                                    OSSemPost(&mdUsart.USART3_s.USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
                                    OSSemPost(&mdUsart.USART3_s.USART_RecvSem,OS_OPT_POST_ALL,&err);/*释放信*/
                                   strcpy(mdUsart.USART3_s.UsartName,"USART3");
					mdUsart.USART3_s.USARTx = USART3;
                                   ;
                                   USART_SendTaskCreate(&mdUsart.USART3_s);
					break;
				}

				
	 case (uint32_t)UART4:
				{
					mdUsart.USART4_s.GPIO_Pin_rxd = USART4_RXD;
					mdUsart.USART4_s.GPIO_Pin_txd = USART4_TXD;
					mdUsart.USART4_s.USART_BaudRate = USART_BaudRate;
					mdUsart.USART4_s.GPIO_USART = USART4_PORT;
					mdUsart.USART4_s.USARTUserDel = CallBackFun;
					/*用户回掉函数注册*/
					
					USARTGpioConfig(&mdUsart.USART4_s);/*io口配置*/
					USARTNvicConfig(UART4_IRQn,0,0);
					USARTConfig(UART4,&mdUsart.USART4_s);
					OSSemCreate(&mdUsart.USART4_s.USART_SendSem, "USART4_SendSem", 1, &err);
					OSSemCreate(&mdUsart.USART4_s.USART_RecvSem, "USART4_RecvSem", 1, &err);
					break;
				}

				
	case (uint32_t)UART5:
				{
					mdUsart.USART5_s.GPIO_Pin_rxd = USART5_RXD_A;
					mdUsart.USART5_s.GPIO_Pin_txd = USART5_TXD_A;
					mdUsart.USART5_s.USART_BaudRate = USART_BaudRate;
					mdUsart.USART5_s.GPIO_USART = USART5_PORT_A;
					mdUsart.USART5_s.USARTUserDel = CallBackFun;
					/*用户回掉函数注册*/
					
					USARTGpioConfig(&mdUsart.USART5_s);/*io口配置*/
					USARTNvicConfig(UART5_IRQn,0,0);
					USARTConfig(UART5,&mdUsart.USART5_s);
					OSSemCreate(&mdUsart.USART5_s.USART_SendSem, "USART5_SendSem", 1, &err);
					OSSemCreate(&mdUsart.USART5_s.USART_RecvSem, "USART5_RecvSem", 1, &err);
					break;
				}


#endif
	}
}
/*****************************************************************************
 * 函 数 名  : UsartGetChar
 * 负 责 人  : Gavin
 * 创建日期  : 2017年5月8日
 * 函数功能  : 从串口缓存中获取一个字符，一般单个字符输入可以使用这个接口
 				如果需要在高速通信，可以使用回调函数
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t UsartGetChar(USARTInfo_s *Usart,uint16_t TimeOut)
{
	uint16_t RecvData = 0;
	OS_ERR err; 
	if(IsQueueEmpty(&Usart->USART_RecvQueue))
	{
		/*暂时不知道怎么协调任务，这里可能需要阻塞机制*/
		//OSSemPend(&Usart->USART_RecvWiteSem,TimeOut,&err);/*超时等待*/
	}
	if(DeQueue(&Usart->USART_RecvQueue,&RecvData))
	{
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);/*继续接收*/
		return RecvData;/*返回该字符*/
	}
}

/*****************************************************************************
 * 函 数 名  : UsartPutCharFush
 * 负 责 人  : Gavin
 * 创建日期  : 2017年5月8日
 * 函数功能  : 向串口中输出一个字符并实时等待发送完成,刷新
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t UsartPutCharFush(uint8_t ch)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
	return(ch);
}

/*****************************************************************************
 * 函 数 名  : UsartPutChar
 * 负 责 人  : Gavin
 * 创建日期  : 2017年5月8日
 * 函数功能  : 向串口缓存区中输出一个字符
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t UsartPutChar(USARTInfo_s *Usart,int16_t SendData,uint16_t TimeOut)
{
		OS_ERR err; 
	
         /*如果队列为满则等待*/
   OSSemPend(&Usart->USART_SendSem,OS_OPT_PEND_BLOCKING,TimeOut,NULL,&err);/*等待信号量*/
         
	 if(IsQueueFull(&Usart->USART_SendQueue))
	 {
			OSSemPost(&Usart->USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
			return -1;
	 }
	 if(EnQueue(&Usart->USART_SendQueue,&SendData))
	 {
	     OSSemPost(&Usart->USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
			/*入队列成功*/
			return 1;
	 }
	 /*释放信号量*/
   OSSemPost(&Usart->USART_SendSem,OS_OPT_POST_ALL,&err);/*释放信号量*/
	 
	 return ERROR;
}
/*发送一个字符串*/
int16_t UsartPutString(USARTInfo_s *Usart,uint8_t *SendString,uint16_t Len,uint16_t TimeOut)
{
	uint16_t uiLoop = 0;
	
	if(Len == 0)
	{
		while(*SendString != '\0')
		{
			UsartPutChar(Usart,*SendString,0);
			SendString++;
		}
	}
	else
	{
		while(uiLoop < Len)
		{
			UsartPutChar(Usart,*SendString,0);
			SendString++;
			uiLoop++;
		}
	}
}

/*发送一个字符串*/
int16_t UsartPutString2(USARTInfo_s *Usart,uint8_t *SendString,uint16_t Len,uint16_t TimeOut)
{
	uint16_t uiLoop = 0;
	
	if(Len == 0)
	{
		while(*SendString != '\0')
		{
			//UsartPutChar(Usart,*SendString,0);
			USART_SendData(USART3,*SendString);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET);
			SendString++;
		}
	}
	else
	{
		while(uiLoop < Len)
		{
			//UsartPutChar(Usart,*SendString,0);
			USART_SendData(USART3,*SendString);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET);
			SendString++;
			uiLoop++;
		}
	}
}

/*****************************************************************************
 * 函 数 名  : UsartWaitReciveEnd
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 串口是否接受完成
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t UsartWaitReciveEnd(USARTInfo_s * Usart)
{
	 uint16_t dat = 0;
	/*说明队列中没有接受任何值*/
	if(IsQueueEmpty(&Usart->USART_RecvQueue))
	{
		return 0;
	}
	if(Usart->USART_RecvQueue.QueueTail ==
		Usart->USART_RecvQueue.QueueOldTail)
	{
		/*上一次接受的值和这次接受的值一样，则代表接受完毕*/
		return 1;/*接受完成*/
	}
	
	Usart->USART_RecvQueue.QueueOldTail = 
	Usart->USART_RecvQueue.QueueTail;
	/*置为祥同志*/
	return 0;
}


/*****************************************************************************
 * 函 数 名  : UsartWaitRecviceData
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 等待接受数据，接受成功返回接受到的个数
 * 输入参数  : uint8_t WaitPort     等待端口
               uint16_t RecvDatLen  接受长度
               uint8_t *RecvBuf     接受缓存区
               uint16_t TimeOut     接受超时时间
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int16_t UsartWaitRecviceData(USARTInfo_s * Usart,uint8_t WaitPort,uint16_t RecvDatLen,uint8_t *RecvBuf,uint16_t TimeOut )
{
    OS_ERR err;
    char *IpdStr = NULL;
    char *IpdStr1 = NULL;
    char *StrPort = NULL;
    uint16_t  RecvDatNum = 0;/*接受字符个数*/
    uint8_t Temp[4] = {0};/*最多接受1024字节*/
    uint8_t RecvDatPort = 0;
    while(TimeOut--)
    {
    	/*接受结束*/
	if(UsartWaitReciveEnd(Usart))
	{
		IpdStr = strstr((char *)Usart->USART_RecvQueue.QueueBuf,"+IPD,");
		IpdStr+=5;/*跳转到端口开始位置*/
		if(NULL != IpdStr)
		{
			RecvDatPort = IpdStr[0] - 48;
			
			if(WaitPort == RecvDatPort)
			{		
				IpdStr+=2;
				/*取出接受到的长度信息*/
				IpdStr1 = strchr(IpdStr, ':');/*找到:*/	
				if(IpdStr1 != NULL)
				{
					memcpy(Temp,IpdStr,(IpdStr1 - IpdStr));
					RecvDatNum = atoi(Temp);
					printf("端口:%d接受到数据，数据长度%d\n",RecvDatPort,RecvDatNum);
					IpdStr+=4;
					memset(RecvBuf,0,RecvDatLen);
					if(RecvDatNum <= RecvDatLen)
					{
						memcpy(RecvBuf,IpdStr,RecvDatNum);/*拷贝数据*/
					}
					else
					{
						memcpy(RecvBuf,IpdStr,RecvDatLen);/*拷贝数据*/
					}
                                    /*队列置空*/
					QueueSetEmpty(&Usart->USART_RecvQueue);
					return RecvDatNum;
				}
				else
				{
					return (-1);
				}
			}
			else
			{
				return (-1);
			}
		}
	}
	OSTimeDly(  (OS_TICK    )1, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
    }
    return (-1);
}

/*****************************************************************************
 * 函 数 名  : UsartSendCmd
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t UsartSendCmd(USARTInfo_s *Usart,char *Cmd,char *Restrict,uint16_t len,
                                        int8_t Mode,AtCmdCallBack CallBack,void *cArg1,void *cArg2,uint16_t TimeOut)
{
    OS_ERR err;
    char *Poite = NULL;
	//nt8_t CheckStrLen = strlen()
	if(!len)
	{
		QueueSetEmpty(&Usart->USART_RecvQueue);/*清空*/	
	}
	UsartPutString2(Usart,(uint8_t *)Cmd,len,0);
	while(TimeOut--)
	{
		if(UsartWaitReciveEnd(Usart))
		{
		       
			if((Poite = strstr((char *)Usart->USART_RecvQueue.QueueBuf,Restrict)) != NULL)
			{   
			    if(CallBack != NULL)
			    {
                      CallBack(Poite,cArg1,cArg2);
                 }
				if(Mode)
				{
					QueueSetEmpty(&Usart->USART_RecvQueue);/*清空*/
				}
				return 1;/*返回成功*/
			}
			if((Poite = strstr((char *)Usart->USART_RecvQueue.QueueBuf,"ALREAY CONNECT"))!= NULL)
			{
				/*已经链接*/
				if(CallBack != NULL)
			    {
                      CallBack(Poite,cArg1,cArg2);
                 }
				if(Mode)
				{
					QueueSetEmpty(&Usart->USART_RecvQueue);/*清空*/
				}
				return 1;/*返回成功*/
			}
		}
        
		OSTimeDly(  (OS_TICK    )1, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
	}
	
	return 0;
}



void USART1_IRQHandler(void)
{
	uint16_t BackData = 0;
	
	OS_ERR err; 
  OSIntEnter();    //通知UCOS进入中断
  
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	 { 
	 	BackData = USART_ReceiveData(USART1);
		#ifndef 	UCOS_SUPPORT
	 		USART_ClearITPendingBit(USART1,USART_IT_RXNE);/*这个不需要了，系统会为我们清楚*/
		#endif
		/*这个分支常用于用户快速处理收到的串口信息*/
		if(mdUsart.USART1_s.USARTUserDel != NULL)
		{
			/*执行用户中断处理函数:提供这样的接口，供用户实现自己的协议*/
			mdUsart.USART1_s.USARTUserDel(BackData,0);
		}
		/*这个是常规处理*/
		{
			/*入数组类型队列，不用担心他满，因为内部已经做了循环*/
			EnQueue(&mdUsart.USART1_s.USART_RecvQueue,&BackData);/*数据入队列*/
			/*释放信号量*/
			//mdUsart.USART1_s.USART_RecvQueue.QueueTail++;
			OSSemPost(&mdUsart.USART1_s.USART_RecvWiteSem,2,&err);
		}
	}
	OSIntExit();
}
void USART2_IRQHandler(void)
{
	OS_ERR err; 
	uint16_t BackData;
	
	OSIntEnter();    //通知UCOS进入中断
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	 { 
		BackData = USART_ReceiveData(USART2);
		#ifndef 	UCOS_SUPPORT
	 		USART_ClearITPendingBit(USART2,USART_IT_RXNE);/*这个不需要了，系统会为我们清楚*/
		#endif
		
		if(mdUsart.USART2_s.USARTUserDel != NULL)
		{
			mdUsart.USART2_s.USARTUserDel(BackData,0);/*执行用户中断处理函数*/
		}
		
		{
			/*入数组类型队列，不用担心他满，因为内部已经做了循环*/
			EnQueue(&mdUsart.USART2_s.USART_RecvQueue,&BackData);/*数据入队列*/
			/*释放信号量*/
			OSSemPost(&mdUsart.USART2_s.USART_RecvWiteSem,2,&err);
		}
	 } 
	OSIntExit();
}

#ifdef STM32F10X_HD

void USART3_IRQHandler(void)
{
	OS_ERR err;
	uint16_t BackData;
	
	OSIntEnter();    //通知UCOS进入中断
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	 { 
	 	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		BackData = USART_ReceiveData(USART3);

              #ifndef 	UCOS_SUPPORT
	 		USART_ClearITPendingBit(USART3,USART_IT_RXNE);/*这个不需要了，系统会为我们清楚*/
		#endif
		if(mdUsart.USART3_s.USARTUserDel != NULL)
		{
			mdUsart.USART3_s.USARTUserDel(BackData,0);/*执行用户中断处理函数*/
		}

              /*入数组类型队列，不用担心他满，因为内部已经做了循环*/
	       EnQueue(&mdUsart.USART3_s.USART_RecvQueue,&BackData);/*数据入队列*/
			/*释放信号量*/
	       OSSemPost(&mdUsart.USART3_s.USART_RecvWiteSem,2,&err);
	 } 
	OSIntExit();
}

void UART4_IRQHandler(void)
{
	uint16_t BackData;
	if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)
	 { 
	 	USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		BackData = USART_ReceiveData(UART4);
		if(mdUsart.USART4_s.USARTUserDel != NULL)
		{
			mdUsart.USART4_s.USARTUserDel(BackData,0);/*执行用户中断处理函数*/
		}
	 } 
}

void UART5_IRQHandler(void)
{
	uint16_t BackData;
	if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET)
	 { 
	 	USART_ClearITPendingBit(UART5,USART_IT_RXNE);
		BackData = USART_ReceiveData(UART5);
		if(mdUsart.USART5_s.USARTUserDel != NULL)
		{
			mdUsart.USART5_s.USARTUserDel(BackData,0);/*执行用户中断处理函数*/
		}
	 } 
}
#endif



