/***********************************************************************************
 * 文 件 名   : bsp_Me3000.h
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月17日
 * 文件描述   : 这个文件主要负责描述ME3000模块
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#ifndef _BSP_ME3000_H
#define _BSP_ME3000_H

#include "Bsp_usart.h"
#include "time.h"
#include "bsp_AtCmdCofig.h"
#include "os.h"
#include "Bsp_led.h"
#include "string.h"

#define ME3000_BIND_USART_PORT (&mdUsart.USART2_s)

#define GSM_ON					GPIO_Pin_3
#define GSM_ON_PORT 			        GPIOC
#define GSM_REST 				GPIO_Pin_2
#define GSM_REST_PORT   		GPIOC

#define GSM_READ_SIGNEL_INT							15000					/*当前信号质量检测间隔时间*/


typedef struct tagMe3000
{
	 USARTInfo_s * Usart;/*模块绑定的串口*/
        uint16_t          Me3000ReadyOk;        /*代表模块已经准备好*/
        uint16_t          SingnelValue;            /*信号强度*/
        uint16_t          SingnelBer;               /*信号误码率*/
	 uint8_t       	  PhoneNumber[15];/*本机电话号码*/
        uint8_t            Me3000Online;    /*模块在线情况，代表是开机状态还是关机状态*/
        OS_TCB      taskMe3000TCB;
        CPU_STK     taskMe3000Stk[TASK3_STK_SIZE];
}Me3000;



extern Me3000 mdMe3000;

int8_t Me3000ParmInit();
int8_t Me3000CheckOnline();
int8_t Me3000TaskInit();
int8_t Me3000Open();
int8_t Me3000SoftClose();
void *Me3000ReadModeVersionCallBack(void *arg1,void *arg2,void *arg3);
void *Me3000ReadSingalValueCallBack(void *arg1,void *arg2,void *arg3);
int8_t Me3000ReadSingalValue();
int8_t Me3000SetShowMode();
int8_t Me3000GetPhoneNumber(void);
void * Me3000GetPhoneNumberCallBack(void *arg1,void *arg2,void *arg3);
int8_t Me3000GetHostByName( const char *Name,char *HostBuf);
void *Me3000GetHostByNameCallBack(void *arg1,void *arg2,void *arg3);
#endif