#ifndef BSP_INTERRUPT_H
#define BSP_INTERRUPT_H

#include "BspConfig.h"
#include "os.h"
#include "bsp_Timer.h"

#define INTERRUPT_MAX_NUM  4

#define InterruptChannel0Pin GPIO_Pin_0
#define InterruptChannel0Port GPIOD
#define InterruptChannel0Line EXTI_Line0
#define InterruptChannel0PinSource GPIO_PinSource0
#define InterruptChannel0PortSource GPIO_PortSourceGPIOC
#define InterruptChannel0Irq EXTI0_IRQn

#define InterruptChannel1Pin GPIO_Pin_1
#define InterruptChannel1Port GPIOD
#define InterruptChannel1Line EXTI_Line0
#define InterruptChannel1PinSource GPIO_PinSource1
#define InterruptChannel1PortSource GPIO_PortSourceGPIOC
#define InterruptChannel1Irq EXTI1_IRQn

#define InterruptChannel2Pin GPIO_Pin_2
#define InterruptChannel2Port GPIOD
#define InterruptChannel2Line EXTI_Line2
#define InterruptChannel2PinSource GPIO_PinSource2
#define InterruptChannel2PortSource GPIO_PortSourceGPIOD
#define InterruptChannel2Irq EXTI2_IRQn

#define InterruptChannel3Pin GPIO_Pin_3
#define InterruptChannel3Port GPIOD
#define InterruptChannel3Line EXTI_Line0
#define InterruptChannel3PinSource GPIO_PinSource3
#define InterruptChannel3PortSource GPIO_PortSourceGPIOC
#define InterruptChannel3Irq EXTI3_IRQn

/*中断回掉函数*/
typedef uint16_t (*InterruptCallFunc)(uint16_t *arg1,uint16_t *arg2);

typedef struct tagInterruptNodeInf
{
	GpioInfo InterruptIo;/*中断引脚，注意中断只能处于一个大端口中，否则无法使用*/
	uint32_t ullInterruptCount;/*中断次数统计*/
	uint8_t  ucInterruptFullFlg;/*中断溢出标志*/
	uint32_t ullInterruptLine;	/*中断线*/
	uint8_t  ucInterruptPinSource; /*中断源配置*/
	uint8_t  ucInterruptPortSource;
	uint8_t  ucInterruptIrq;			/*中断通道*/
	InterruptCallFunc func;				/*中断回掉函数*/
}InterruptNodeInf;

typedef struct tagInterruptInfo
{
	InterruptNodeInf sInterruptInfo[INTERRUPT_MAX_NUM];/*最大应该可以支持16组中断*/
}InterruptInfo;

void InterruptInit(uint8_t ucChannel,InterruptCallFunc func,uint8_t ucMastPriority,uint8_t ucSubPriority);
uint32_t InterruptGetCenterCount(InterruptNodeInf *InterruptNode);
/*清除中断计数*/
void IneterruptCleanCount(InterruptNodeInf *InterruptNode);
extern InterruptInfo mdInterruptInfo;
#endif