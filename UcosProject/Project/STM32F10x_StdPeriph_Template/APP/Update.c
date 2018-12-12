/***********************************************************************************
 * 文 件 名   : Update.c
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月6日
 * 文件描述   : 主要用于完成远程升级处理，和服务器保持长链接，
 				加入心跳机制
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "Update.h"


static  OS_TCB      taskUpdateTCB;
static  CPU_STK     taskUpdateStk[TASK3_STK_SIZE];

/*****************************************************************************
 * 函 数 名  : UpdateTask
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月6日
 * 函数功能  : 系统远程更新任务
 * 输入参数  : void *p_arg  输入参数
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void UpdateTask(void *p_arg)
{
	/*初始化*/
	uint8_t *Ptr;
	OS_ERR err;
	char buf[20] = {0};
	int16_t len = 0;
	int8_t ConnectStatus;
	sCmdPaket CmdPakets;
	
	/*获取一个空闲的socket*/
	do
	 {
			OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		   ConnectStatus = ESP8266_TcpClientConnect(1,"192.168.31.33",8001);
		    if(ConnectStatus)
		    {
			printf("远程更新服务器链接成功\n");
		    }
			
	 }while(!ConnectStatus);
	
	/*链接到服务器*/
	while(1)
	{
#if 0
		len = ESP8266_WaitRecviceData(1,sizeof(buf),buf,100000);/*永不超时*/
		if(len)
		{
			printf("%d\n",len);
			QueueSetEmpty(&mdEsp8266.Usart->USART_RecvQueue);/*缓存队列*/
			
		}
#else
		OSTimeDly(  (OS_TICK    )100, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		CmdPakets.uiHead = 0x7e7e;
		CmdPakets.uiEnd = 0x5aa5;
		CmdPakets.uiPaketCmd++;
		ESP8266_TcpSend(1,(char *)&CmdPakets,sizeof(sCmdPaket));
#endif
	}
}

void UpdateTaskInit()
{
	OS_ERR err;

	OSTaskCreate(   (OS_TCB     *)&taskUpdateTCB,
                    (CPU_CHAR   *)"UpdateTask",
                    (OS_TASK_PTR)UpdateTask,
                    (void       *)0,
                    (OS_PRIO    )TASK3_PRIO,
                    (CPU_STK    *)&taskUpdateStk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);
	
}

