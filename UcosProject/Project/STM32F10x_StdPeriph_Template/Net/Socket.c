/*网络层描述，主要是实现TCP通信，处于网络驱动层*/
#include "Socket.h"

Socket mdSocket;/*网络层*/

static  OS_TCB      taskNetTCB;
static  CPU_STK     taskNetStk[TASK3_STK_SIZE];
/*该任务主要用于维护soket的连接情况，wifi的连接情况由wifi自行维护*/
void* TaskNet(void *p_arg)
{
	OS_ERR 		err;
	
	ESP8266_Init();/*wifi模块初始化*/
    
	mdSocket.ucCenterNetState |= NET_LINK_OK;/*WiFi连接成功*/
       
	while(1)
	{
	       /*每各一段时间询问一下没款是否有响应*/
		OSTimeDly(  (OS_TICK    )1000, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
	}
}

/*****************************************************************************
 * 函 数 名  : SocketLevelInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月5日
 * 函数功能  : 网络层初始化，主要是控制上网设备进行联网出准备状态
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t SocketLevelInit( void )
{
    /*上网设备初始化，、
		主要需要配置模块可以处于上网状态，这里主要时回掉用户的注册函数*/
		/*创建一个任务，配置wifi或者gsm模块并维护状态*/
		OS_ERR 		err;
	
		OSTaskCreate(   (OS_TCB     *)&taskNetTCB,
                    (CPU_CHAR   *)"TaskNet",
                    (OS_TASK_PTR)TaskNet,
                    (void       *)0,
                    (OS_PRIO    )TASK3_PRIO,
                    (CPU_STK    *)&taskNetStk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);
}
/*****************************************************************************
 * 函 数 名  : SocketCreate
 * 负 责 人  : xiegui
 * 创建日期  : 2018年4月6日
 * 函数功能  : 创建一个服务器
 * 输入参数  : int domain    无
               int type      无
               int protocol  无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int SocketServerCreate(int domain, int type, int protocol)
{
	
}
/*****************************************************************************
 * 函 数 名  : SocketClientCreate
 * 负 责 人  : xiegui
 * 创建日期  : 2018年4月6日
 * 函数功能  : 客户端创建,并连接到服务器，并开启一个线程来维护连接
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int SocketClientCreate()
{
	return mdSocket.SocketClientCreate();
}
/*****************************************************************************
 * 函 数 名  : SocketClose
 * 负 责 人  : xiegui
 * 创建日期  : 2018年4月6日
 * 函数功能  : 关闭一个描述符
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int SocketClose(int iSocketFd)
{
    
}
/*****************************************************************************
 * 函 数 名  : SocketRead
 * 负 责 人  : xiegui
 * 创建日期  : 2018年4月6日
 * 函数功能  : 网络读取
 * 输入参数  : int iSocketFd  输入描述符号
               char *Buff  
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int SocketRead( int iSocketFd, char *Buff ,int iSize)
{
    
}
/*****************************************************************************
 * 函 数 名  : SocketWrite
 * 负 责 人  : xiegui
 * 创建日期  : 2018年4月6日
 * 函数功能  : 网络写
 * 输入参数  : int iSocketFd  输入网络描述符
               char *Buff  缓存区
               int iSize  写入纸数
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int SocketWrite( int iSocketFd, char *Buff, int iSize )
{
    
}