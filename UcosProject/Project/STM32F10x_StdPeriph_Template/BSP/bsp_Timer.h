/***********************************************************************************
 * 文 件 名   : bsp_Timer.h
 * 负 责 人   : xiegui
 * 创建日期   : 2017年11月28日
 * 文件描述   : 这个是定时器驱动
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#ifndef BSP_TIMER_H
#define BSP_TIMER_H


#include "BspConfig.h"
#include "stdio.h"
#include "os.h"

typedef uint16_t (*TimerCallFunc)(uint16_t *arg1,uint16_t *arg2);

#define MAX_TIMER_TASK   8
#define TIMER_TASK_BUF_USE  1
#define TIMER_TASK_BUF_FREE  0

/*定时任务模式*/
#define TIMER_TASK_ONECE 0
#define TIMER_TASK_FOREVER 1

typedef struct tagTimerCallFunInfo
{
	uint32_t uiDelayMs; /*定时任务定时时间:单位BastMs*/
	uint32_t uiDelayCount;/*定时计数器*/
	TimerCallFunc Func; /*定时回掉时间*/
	uint16_t arg1;			/*参数1*/
	uint16_t arg2;		  /*参数2*/
	uint8_t  TimerNodeStatus; /*当前状态*/
	uint8_t  TimerMode; /*0，单次执行，1，多次执行*/
}TimerCallFunInfo;

typedef struct tagTimerNodeInfo
{
	/*预分频器*/
	uint16_t TimerPeriod;
	uint16_t TimerPrescaler;
	/*计数模式*/
	uint16_t TimerCounterMode;
	uint8_t TimerUseSate;
	uint8_t TimerInitOk;
	uint16_t TimetBaseMs;/*定时器的基准Ms时间*/
	/*定时器使用状态，如果已经被初始化了则不运行再进行初始化
	除非强制初始化*/
	/*定时器中断服务执行用户函数，最好不是耗时间的函数*/
	TimerCallFunc IntBackFun; /*主回掉函数*/
	/*回掉函数的参数*/
	uint16_t TimerCallFunArg1;
	uint16_t TimerCallFunArg2;
	TimerCallFunInfo TimerTask[MAX_TIMER_TASK];/*定时任务，最大5个*/
	
}TimerNodeInfo;
/*单个定时器需要保存的信息*/

typedef struct tagTimerInfo
{
	TimerNodeInfo Timer1;/*定时器1*/
	TimerNodeInfo Timer2;/*定时器1*/
	TimerNodeInfo Timer3;/*定时器1*/
}TimerInfo;

void TimerAdvanceInit(TIM_TypeDef* Timer,uint16_t TimerPeriod,uint16_t TimerPrescaler,
				uint16_t TimerMode,uint16_t TimerClockDivision,
				TimerCallFunc callFunc,uint8_t ucFlage,uint8_t ucPwmEnable);


void  TimerInit(TIM_TypeDef* Timer,uint16_t uiMsDelay,TimerCallFunc callFunc,uint8_t ucFlage,uint8_t ucPwmEnable);
int8_t TimerTaskAdd(TimerNodeInfo *TimerNode,TimerCallFunc hook,
										uint16_t arg1,uint16_t arg2,uint8_t TaskMode,
										uint32_t uiTime);
int8_t TimerTaskDelete(TimerNodeInfo *TimerNode,int8_t TimerTaskId);

extern TimerInfo mdTimer;

#endif
