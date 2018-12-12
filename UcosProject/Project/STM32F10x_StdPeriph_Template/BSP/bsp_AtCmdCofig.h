/***********************************************************************************
 * 文 件 名   : bsp_AtCmdCofig.h
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月17日
 * 文件描述   : 这个文件主要用来枋使用at指令配置的设备公共描述文件
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef _BSP_ATCMDCONFIG_H
#define _BSP_ATCMDCONFIG_H

#include "Bsp_usart.h"

typedef void * (*AtCmdCallBack)(void *arg1,void *arg2,void *arg3);

typedef struct tagAtCmdParInfo
{	
	char CmdInfo[30];               /*命令名称*/
	char CmdRsp[30];                /*命令响应*/
	uint8_t CmdMode;	              /**接受到数据后的模式*/
	uint16_t CmdTimeOut;        /*超时时间*/
	AtCmdCallBack CallBack;   /*回掉函数*/
}AtCmdParInfo;



#endif