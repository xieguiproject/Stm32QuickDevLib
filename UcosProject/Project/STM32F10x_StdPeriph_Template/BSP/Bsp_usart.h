#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "BspConfig.h"
#include "stdio.h"
#include "os.h"
#include "Lib/Queue/Queue.h"
#include "bsp_atCmdCofig.h"
#define RECV_IS_ING	 			0X02			/*正在接受*/
#define RECV_NEW_DATA   		0X04			/*接受到新数据*/
#define RECV_FULL				0X08			/*接受超限*/
#define SEND_IS_ING				0X10			/*正在发送*/
#define SEND_OVER				0X20			/*发送完成*/
#define SEND_ERROR				0X40			/*发送失败*/


/*这些宏应该是和硬件相关的，只有在修改主系列时才会修改的问题*/
/*串口1*/
#define USART1_PORT        			 GPIOA
#define USART1_TXD 					GPIO_Pin_9
#define USART1_RXD					GPIO_Pin_10
/*串口2*/
#define USART2_PORT 				GPIOA
#define USART2_TXD 					GPIO_Pin_2
#define USART2_RXD					GPIO_Pin_3
/*串口3*/
#ifdef STM32F10X_HD
/*串口3*/
#define USART3_PORT 				GPIOB
#define USART3_TXD 					GPIO_Pin_10
#define USART3_RXD					GPIO_Pin_11

/*串口4*/
#define USART4_PORT					GPIOC
#define USART4_TXD 					GPIO_Pin_10
#define USART4_RXD					GPIO_Pin_11

/*串口5*/
#define USART5_PORT_A				GPIOC
#define USART5_TXD_A 				GPIO_Pin_12
#define USART5_RXD_A				GPIO_Pin_2

#define USART5_PORT_B				GPIOD
#define USART5_TXD_B 				GPIO_Pin_12
#define USART5_RXD_B				GPIO_Pin_2


#endif



typedef uint16_t (*USARTCallFunc)(uint16_t arg1,uint16_t arg2);
typedef struct tagUSARTInfo_s
{
	char       		UsartName[10];		/*串口名称*/
	USART_TypeDef*  USARTx;			/*对应串口库中的USART1、USART2*/
	uint16_t 		GPIO_Pin_txd;	/*txd引脚*/
	uint16_t 		GPIO_Pin_rxd;	/*rxd引脚*/
	GPIO_TypeDef* 	GPIO_USART;		/*串口在那个端口上*/
	uint32_t 		USART_BaudRate; /*波特率*/
	OS_SEM			USART_SendSem;	/*串口发送信号量,主要是用来限制单个任务访问串口*/
	OS_SEM			USART_RecvSem;	/*串口接收信号量*/
	OS_SEM 			USART_RecvWiteSem;	/*串口接收等待信号量*/
	USARTCallFunc   USARTUserDel;	/*回调函数*/
	Queue			USART_SendQueue;/*发送队列2k*/
	
	Queue			USART_RecvQueue;/*发送队列2k*/
	OS_TCB     taskSendTCB;			/*发送任务tcb*/
  CPU_STK    taskSendStk[TASK3_STK_SIZE];/*发送任务占空间*/
}USARTInfo_s;	

typedef struct tagUSART_s
{
	USARTInfo_s USART1_s;
	USARTInfo_s USART2_s;
#ifdef STM32F10X_HD
	USARTInfo_s USART3_s;
	USARTInfo_s USART4_s;
	USARTInfo_s USART5_s;
#endif
}USART_s;

extern USART_s mdUsart;

void USARTInit(USART_TypeDef* USARTx,uint32_t USART_BaudRate,
					USARTCallFunc CallBackFun);
int8_t UsartPutChar(USARTInfo_s *Usart,int16_t SendData,uint16_t TimeOut);
int8_t UsartWaitReciveEnd(USARTInfo_s * Usart);/*查询对应串口是否接受完成*/
int16_t UsartWaitRecviceData(USARTInfo_s * Usart,uint8_t WaitPort,uint16_t RecvDatLen,uint8_t *RecvBuf,uint16_t TimeOut );
/*等待接受数据*/
int16_t UsartPutString(USARTInfo_s *Usart,uint8_t *SendString,uint16_t Len,uint16_t TimeOut);
int8_t UsartSendCmd(USARTInfo_s *Usart,char *Cmd,char *Restrict,uint16_t len,
                                        int8_t Mode,AtCmdCallBack CallBack,void *cArg1,void *cArg2,uint16_t TimeOut);
int8_t UsartPutCharFush(uint8_t ch);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void UART4_IRQHandler(void);
extern void UART5_IRQHandler(void);



extern USART_s mdUsart;

#define LOG_DEBUG				1
#define LOG_RUN					2
#define LOG_CLOSE				3

#define LOG_CENTER_LEVE	 LOG_DEBUG	/*当前打印等级为调试模式*/

#define LogMsgStr(LogLevel,MsgStr)\
{\
	if(LogLevel == LOG_CENTER_LEVE)\
	{\
		printf(MsgStr);\
	}\
}
#define LogMsgInt(LogLevel,FormatStr,MsgInt)\
{\
	if(LogLevel == LOG_CENTER_LEVE)\
	{\
		printf(FormatStr,MsgInt);\
	}\
}
#endif
