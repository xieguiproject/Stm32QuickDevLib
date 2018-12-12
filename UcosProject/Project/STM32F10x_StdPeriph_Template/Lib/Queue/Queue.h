/***********************************************************************************
 * 文 件 名   : Queue.h
 * 负 责 人   : Gavin
 * 创建日期   : 2017年5月8日
 * 文件描述   : 这个文件主要是用来实现环形队列，该队列为线性环形队列，主要解
                决高速设备去低速设备之间的缓存问题
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "stm32f10x.h"

#define QueueStandardType           0 /*标准队列*/
#define QueueArryType			1 /*数组型队列*/



typedef struct s_Queue
{ 
	uint8_t  QueueBuf[1024];
	uint16_t QueueSize;	/*任务队列大小*/
	uint8_t  QueueType;	/*队列类型，0完全循环队列 1:循环数组类型队列*/
	uint16_t QueueHead;	/*任务队列头部*/
	uint16_t QueueTail;	/*任务队列尾部*/
	uint16_t QueueOldTail;/*上一次队列尾部*/
}Queue;

uint16_t EnQueue(Queue *Queues,uint16_t *arg1);
uint16_t DeQueue(Queue *Queues,uint16_t *arg1);
int IsQueueEmpty(Queue *Queues);
int IsQueueFull(Queue *Queues);
int QueueSetEmpty(Queue *Queues);


