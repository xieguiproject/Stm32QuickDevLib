/***********************************************************************************
 * 文 件 名   : LockControl.c
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月12日
 * 文件描述   : 锁控制应用，主要针对摩布图书编写
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#include "LockControl.h"

/*****************************************************************************
 * 函 数 名  : LockControlInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月12日
 * 函数功能  : 锁控制模块初始化函数
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/

LockControl mdLockControl;

/*****************************************************************************
 * 函 数 名  : CalcCrc
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 计算CRC参数
 * 输入参数  : char *arg1  计算CRC刂
               int sizes   长度
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint8_t CalcCrc(char *arg1,int sizes)
{
	uint8_t uiLoop = 0;
	uint8_t ucCrcValue = arg1[0];

	if(arg1 == NULL)
	{
		return 0;
	}

	if(sizes == 0 || sizes == 1)
	{
		return ucCrcValue;
	}
	
	for(uiLoop = 1;uiLoop < sizes;uiLoop++)
	{
		ucCrcValue ^=  arg1[uiLoop];
	}
	
	return ucCrcValue;
}

/*****************************************************************************
 * 函 数 名  : Bit2Swap
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : 这个函数50口专用函数，负责将bit0和bit1进行交换
                                这是由于所状态的BUG造成的。
 * 输入参数  : uint8_t dat  待交换的字节数
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint8_t Bit2Swap(uint8_t dat)
{
	utype temp;
	utype temp2;
	
	temp.data_char = dat;
	
	temp2.data_bits.bit0 = temp.data_bits.bit1;
	temp2.data_bits.bit1 = temp.data_bits.bit0;
	
	return temp2.data_char;
}

/*****************************************************************************
 * 函 数 名  : BitSwap
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : 这个函数是50口专用函数，负责将一个字节数
                                高低位全部换位
 * 输入参数  : uint8_t dat  输入待交换的数据
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint8_t BitSwap(uint8_t dat)
{
	utype temp;
	utype temp2;
	
	temp.data_char = dat;
	
	temp2.data_bits.bit0 = temp.data_bits.bit7;
	temp2.data_bits.bit1 = temp.data_bits.bit6;
	temp2.data_bits.bit2 = temp.data_bits.bit5;
	temp2.data_bits.bit3 = temp.data_bits.bit4;
	temp2.data_bits.bit4 = temp.data_bits.bit3;
	temp2.data_bits.bit5 = temp.data_bits.bit2;
	temp2.data_bits.bit6 = temp.data_bits.bit1;
	temp2.data_bits.bit7 = temp.data_bits.bit0;
	
	return temp2.data_char;
}

/*****************************************************************************
 * 函 数 名  : LockControlGetFreeMsgBuf
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : void
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
 LockMsgType *LockControlGetFreeMsgBuf()
{
    uint8_t ucLoop = 0;

    for(ucLoop = 0;ucLoop < LOCK_MSG_NUM;ucLoop++)
    {
        if(mdLockControl.MsgBuf[ucLoop].stau == LOCK_MSG_FREE)
         {
            mdLockControl.MsgBuf[ucLoop].stau = LOCK_MSG_USE;
            return &mdLockControl.MsgBuf[ucLoop];
         }
    }
    return NULL;
}

/*****************************************************************************
 * 函 数 名  : LockControlFreeMsgBuf
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 释放消息缓冲区
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlFreeMsgBuf( LockMsgType * MsgBuf)
{
    if(MsgBuf != NULL)
    {
        MsgBuf->arg1 = 0;
        MsgBuf->arg2 = 0;
        MsgBuf->cmd =  0;
        MsgBuf->stau = LOCK_MSG_FREE;

        return 1;
    }

    return 0;
}

/*****************************************************************************
 * 函 数 名  : LockControlParmInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月12日
 * 函数功能  : 锁控制模块参数初始化
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlParmInit( void )
{
	uint8_t ucLoop = 0;
	OS_ERR err;
    
    	mdLockControl.Usart = LOCK_CONTROL_USART;/*串口绑定*/

	USARTInit(USART3,9600,NULL);
       
	mdLockControl.Usart->USART_RecvQueue.QueueType = QueueArryType;

	for(ucLoop = 1; ucLoop <= BOARD_NUM; ucLoop++)
	{
		mdLockControl.sAllBoardInfo[ucLoop].ucBoardAddr = ucLoop;/*锁地址初始化*/
	}

        /*消息队列初始化,可以存放10条消息*/
        OSQCreate((OS_Q*)&mdLockControl.msg,(CPU_CHAR*)"LockMsg",(OS_MSG_QTY)10,(OS_ERR*)&err);
}

/*****************************************************************************
 * 函 数 名  : LockControlTaskInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月12日
 * 函数功能  : 锁控制器任务初始化
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlTaskInit( void )
{
	OS_ERR err;

	OSTaskCreate(   (OS_TCB     *)&mdLockControl.taskTCB,
                    (CPU_CHAR   *)"LockControlTask",
                    (OS_TASK_PTR)ControlControlTask,
                    (void       *)0,
                    (OS_PRIO    )TASK3_PRIO,
                    (CPU_STK    *)&mdLockControl.taskStk[0],
                    (CPU_STK_SIZE)TASK_MINI_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK_MINI_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);

	printf("\n锁控制模块控制任务创建成功\n");
	
}

int8_t LockControlInit( void )
{
	/*参数初始化*/
	LockControlParmInit();/*参数初始化*/

	/*任务初始化*/
	LockControlTaskInit();
	
	printf("\n锁控制模块初始化成功\n");

}
/*****************************************************************************
 * 函 数 名  : LockControlOpenALock
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 打开指定的锁
 * 输入参数  : uint8_t BoradAddr  要开启的锁的板卡刂
               uint8_t LockAddr   锁地址
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlOpenALock(uint8_t BoardAddr,uint8_t LockAddr)
{
    	OpenBoardLock OpenBoardLockCmd;
	/*这个函数只能由锁控制器调用，否则会发生原子操作*/
	if(BoardAddr < 1 || BoardAddr > BOARD_NUM)
	{
		return 0;
	}
    
	memset(&OpenBoardLockCmd,0,sizeof(OpenBoardLock));
        
	if(LockAddr < 1 || LockAddr > BOARD_LOCK_NUMS)
	{
		return 0;
	}
	
	OpenBoardLockCmd.ucHead = OPEN_BOARD_LOCK_HEAD;
	OpenBoardLockCmd.ucBoardAddr = BoardAddr;
	OpenBoardLockCmd.ucLockAddr = LockAddr;
	OpenBoardLockCmd.ucState = 0x11;
	OpenBoardLockCmd.ucCrc = CalcCrc((char *)&OpenBoardLockCmd,(sizeof(OpenBoardLock) -1));
    
	UsartPutString(mdLockControl.Usart,(char *)&OpenBoardLockCmd,sizeof(OpenBoardLock),100);
    
	return 1;
}

/*****************************************************************************
 * 函 数 名  : LockControlOpenALockMsg
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 该函数饕提供外部任务进行调用，
 他会发送一个消息到锁控制模块，
               这样梢避免原子操作和精准氖序控制
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlOpenALockMsg(uint8_t BoradAddr,uint8_t LockAddr)
{
    LockMsgType *MsgBuf = NULL ;
    OS_ERR err;
     
    MsgBuf = LockControlGetFreeMsgBuf();
    
    if(NULL != MsgBuf)
    {
        MsgBuf->cmd = OPEN_BOARD_LOCK_HEAD;/*开锁*/
        MsgBuf->arg1 = BoradAddr; /*板卡地址*/
        MsgBuf->arg2 = LockAddr; /*锁地址*/
        
        printf("开锁消息发送:板卡地址:0x%x,锁地址:0x%x\n",BoradAddr,LockAddr);
        
        OSQPost((OS_Q*)&mdLockControl.msg,(void*)MsgBuf,(OS_MSG_SIZE)(sizeof(LockMsgType)),
            (OS_OPT)OS_OPT_POST_FIFO,(OS_ERR*)&err); 
    }
}

/*****************************************************************************
 * 函 数 名  : ReadAllLock
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 查询指定板卡的锁状态
 * 输入参数  : uint8_t BoardAddr  查询板卡地址
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint16_t LockControlReadAllLock(uint8_t BoardAddr)
{
	ReadBoardLockState ReadLockState;
	
	memset(&ReadLockState,0,sizeof(ReadBoardLockState));
	
	ReadLockState.ucHead = READ_BOARD_LOCK_HEAD;
	ReadLockState.ucBoardAddr = BoardAddr;
	ReadLockState.ucLockAddr = 0x00;
	ReadLockState.ucCmd = 0x33;
	ReadLockState.ucCrc = CalcCrc((char *)&ReadLockState,(sizeof(ReadBoardLockState) - 1));

	UsartPutString(mdLockControl.Usart,(char *)&ReadLockState,sizeof(ReadBoardLockState),100);
	return 1;
}

/*****************************************************************************
 * 函 数 名  : LockCheckReadLockRespParm
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月13日
 * 函数功能  : 检查查询板卡状态的响应帧问
 * 输入参数  : char *Buf  输入缓存区域
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockCheckRspParm(uint8_t Head,char *LockRspParm,uint8_t sizes)
{
    uint8_t CrcTemp = 0;/*crc值*/
    if(LockRspParm[0] == Head)
    {
        CrcTemp = CalcCrc((char *)&LockRspParm[0],sizes);
        if(CrcTemp == LockRspParm[sizes])
        {
            return 1;
        }
    }
    
    return 0;
}

/*****************************************************************************
 * 函 数 名  : LockControlLockStateDel
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : 控制板卡状态维护处理
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockControlLockStateDel(ReadBoardLockStateRsp *aBoardLockState)
{
#if BOARD_LOCK_NUMS == BOARD_LOCK_NUM_24
    uint8_t uiLoop = 0;
    uint32_t LockState;
    BoardInfo *BoardInfos;
    
    if(aBoardLockState == NULL)
    {
        return 0;/*指针出错*/
    }
    
    BoardInfos =  (BoardInfo *)&mdLockControl.sAllBoardInfo[aBoardLockState->ucBoardAddr];
    
    LockState = ((uint32_t)aBoardLockState->ucState1 * 256 + aBoardLockState->ucState2) * 
                      256 + aBoardLockState->ucState3;
    
	for(uiLoop = 1;uiLoop <= BOARD_LOCK_NUMS; uiLoop++)
	{
		if((LockState & (0x00000001 << (uiLoop - 1))))
		{
		      BoardInfos->ucBoardLockState[uiLoop] = BOARD_LOCK_CLOSE_STATE;
		}
		else
		{	
			BoardInfos->ucBoardLockState[uiLoop]  =  BOARD_LOCK_OPEN_STATE;
		}
    }
    LedClose(LED2);
    return 1;
#else
    uint8_t uiLoop = 0;
    uint32_t Temp = 0;
    uint32_t Temp2 =0;
    BoardInfo *BoardInfos;
    
    if(aBoardLockState == NULL)
    {
        return 0;/*指针出错*/
    }
    
    BoardInfos =  (BoardInfo *)&mdLockControl.sAllBoardInfo[aBoardLockState->ucBoardAddr];
    
    Temp2 = ((uint32_t)(Bit2Swap(aBoardLockState->ucState7) << 16)) | 
                  (uint32_t)(BitSwap(aBoardLockState->ucState6) << 8) |  
                  (uint32_t)(BitSwap(aBoardLockState->ucState5) << 0);
	
    Temp = (uint32_t)(BitSwap(aBoardLockState->ucState4) << 24) | 
                (uint32_t)(BitSwap(aBoardLockState->ucState3) << 16) | 
                (uint32_t)(BitSwap(aBoardLockState->ucState2) << 8)  |
	         (uint32_t)(BitSwap(aBoardLockState->ucState1) << 0);
    
	for(uiLoop = 1;uiLoop <= 32; uiLoop++)
	{
		if((Temp & ((uint32_t)0x01 << (uiLoop - 1))))
		{
			BoardInfos->ucBoardLockState[uiLoop] =  BOARD_LOCK_CLOSE_STATE;
		}
		else
		{
			BoardInfos->ucBoardLockState[uiLoop]=  BOARD_LOCK_OPEN_STATE;
		}
	}
	
	for(uiLoop = 33;uiLoop < BOARD_LOCK_NUMS; uiLoop++)
	{
		if((Temp2 & ((uint32_t)0x01 << (uiLoop - 33))))
		{
			BoardInfos->ucBoardLockState[uiLoop] =  BOARD_LOCK_CLOSE_STATE;
		}
		else
		{
			BoardInfos->ucBoardLockState[uiLoop] =  BOARD_LOCK_OPEN_STATE;
		}
	}
    LedClose(LED2);
    return 1;
#endif
}

/*****************************************************************************
 * 函 数 名  : ControlControlTask
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月12日
 * 函数功能  : 锁控制制器管理任务
 * 输入参数  : void *p_arg  任务输入信息
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void ControlControlTask(void *p_arg)
{
    OS_ERR err;
		ReadBoardLockStateRsp *aBoardLockState = NULL;
    LockMsgType *msg;
    uint8_t  ucLoop = 0;
    uint16_t uiJoop = 0;
    char buf[sizeof(ReadBoardLockStateRsp)] = {0};
    /*该函数主要实现开锁、锁状态管理等操作*/
    /*查询板卡数量*/
   while(1)
    {
#if 1
        /*首先等待消息队列中的开锁命令*/
        if(NULL != (msg = OSQPend((OS_Q*)&mdLockControl.msg,(OS_TICK)0,(OS_OPT)OS_OPT_PEND_NON_BLOCKING,
                (OS_MSG_SIZE*)(sizeof(LockMsgType)),(CPU_TS*)0,(OS_ERR*)&err)))
         {
            /*接受到一个消息*/
            if(msg->cmd == OPEN_BOARD_LOCK_HEAD)
            {
                printf("#####开锁命令:板卡地址0x%x,锁地址0x%x#####\n",
                         msg->arg1,msg->arg2);
                
                LockControlOpenALock(msg->arg1,msg->arg2);/*开锁*/
                /*释放该消息*/
                LockControlFreeMsgBuf(msg);
#ifdef CONTROL_LOCK_BORAD_RSP_TYPE
                OSTimeDly(  (OS_TICK    )1200, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
#else
               OSTimeDly(  (OS_TICK    )1000, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err); 
#endif
            }
         }
#endif
        /*查询每块板子的锁状态*/
        for(ucLoop = 1; ucLoop <= BOARD_NUM;  ucLoop++)
        {
            LockControlReadAllLock(ucLoop);/*发送查询命令*/
            LedOpen(LED2);
            /**等待*/
            OSTimeDly(  (OS_TICK    )(READ_LOCK_STAT_INT / 4), /*50ms,对应实际值50 * 4 = 200ms*/
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
            
            if(UsartWaitReciveEnd(mdLockControl.Usart))
             {
                for(uiJoop = 0;uiJoop < sizeof(mdLockControl.Usart->USART_RecvQueue.QueueBuf);uiJoop++)
                {
                    if(mdLockControl.Usart->USART_RecvQueue.QueueBuf[uiJoop] == READ_BOARD_LOCK_HEAD)
                    {
                        memcpy(buf,(char *)&mdLockControl.Usart->USART_RecvQueue.QueueBuf[uiJoop],sizeof(ReadBoardLockStateRsp));
                        /*检擦是否符合协议*/
                        if(LockCheckRspParm(READ_BOARD_LOCK_HEAD,buf,(sizeof(ReadBoardLockStateRsp) -1)))
                        {	
			        aBoardLockState = (ReadBoardLockStateRsp *)buf; 
				LockControlLockStateDel(aBoardLockState);
				
                            memset((char *)&mdLockControl.Usart->USART_RecvQueue.QueueBuf[uiJoop],0,sizeof(ReadBoardLockStateRsp));
                        }
                     }
                 }
                
                QueueSetEmpty(&mdLockControl.Usart->USART_RecvQueue);
                 
                OSTimeDly(  (OS_TICK    )100, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
            }
        }
				
    }
}

/*****************************************************************************
 * 函 数 名  : LockShowAllLockState
 * 负 责 人  : Gavin
 * 创建日期  : 2017年5月1日
 * 函数功能  : 查看所有的锁状态
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : uint16_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t LockShowAllLockState()
{
	uint16_t uiLoop = 1;
    printf("\r\n");
    for(uiLoop = 1;uiLoop < BOARD_LOCK_NUMS;uiLoop++)
    {
	     printf(" %d--0x%.2x ",uiLoop,mdLockControl.sAllBoardInfo[0x01].ucBoardLockState[uiLoop]);
    }
	printf("\r\n");
}
