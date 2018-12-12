#ifndef _BSP_ULTRASONIC_SENSER_H
#define _BSP_ULTRASONIC_SENSER_H


#include "stm32f10x.h"			
#include "os.h"				/*需要基于操作系统*/
#include "BspConfig.h"
#include "bsp_Timer.h"		/*需要使用定时器*/
#include "bsp_Interrupt.h" /*需要使用外部中断*/
#include "bsp_SysDelay.h"

/*超声波传感器测量原理，
首先发起超声波信号，并开启一个定时器，和中断，收到中断后结束计时器，并计算出距离*/

#define UltrasonicSenserMaxCm 450			/*最远450cm*/
#define UltrasonicSenserMinCm 2				/*最小2cm*/

#define UltrasonicSenserTxdPort GPIOA
#define UltrasonicSenserTxd GPIO_Pin_1

#define UltrasonicSenserRxdPort GPIOA
#define UltrasonicSenserRxd GPIO_Pin_0

#define UltrasonicSenserBindTimer &mdTimer.Timer2

#define UltrasonicSenser1BindInter &mdInterruptInfo.sInterruptInfo[0]
#define UltrasonicSenser2BindInter &mdInterruptInfo.sInterruptInfo[1]

typedef struct tagUltrasonicSenserNodeInfo_s
{
	GpioInfo        UltrasonicSenserTx;	/*发送引脚*/
	GpioInfo		UltrasonicSenserRx;	/*超声波接受应交*/
	InterruptNodeInf *BindInterruptNode;/*绑定到外部中断的节点*/
	TimerNodeInfo    *BindTimerNode;/*绑定到的定时器*/ 
	uint32_t        uliTimerMsCount;/*定时器ms计数器*/
	uint32_t        uliTimerBeginUsCount;/*开始检测回波之前定时器的us数，因为不能去清除
										 定时器的值，只能记录下来，*/
	uint8_t         ucTimerFlag;
	int8_t          cTimerTaskId;
	volatile float           fCenterCm;		/*当前距离*/
	volatile float 					fOldCm;
}UltrasonicSenserNodeInfo;

typedef struct UltrasonicSenserInfo
{
  UltrasonicSenserNodeInfo SenserNode[2];//两路
  OS_SEM	 Sem;
  OS_TCB     taskUltrasonicSenserTCB;	
  CPU_STK    taskUltrasonicSenserStk[TASK3_STK_SIZE];
}UltrasonicSenserInfo;

void OpenCheckSenserEchoTimer(uint8_t SenserId);
void CloseCheckSenserEchoTimer(uint8_t SenserId);
void UltrasonicSenserInit();
float GetCenterSenserCm(uint8_t SenserId);/*对外提供的距离获取*/

extern volatile UltrasonicSenserInfo mdUltrasonicSenserInfo;

#endif