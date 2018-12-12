/*-------------------------------------------------------------------------

							软件主体

    
-------------------------------------------------------------------------*/

#include "bsp.h"
#include "App.h"
#include "stdio.h"
#include "stm32f10x_gpio.h"

extern void MainTask(void);

static  OS_TCB      taskStartTCB;
static  CPU_STK     taskStartStk[STARTUP_TASK_STK_SIZE]; 		//启动任务的程序空间

static OS_TCB		taskLogicTCB;
static  CPU_STK     taskLogicStk[TASK3_STK_SIZE];/*逻辑任务栈空间*/

static OS_TCB       taskOnenetTCB;
static  CPU_STK     taskOnenetStk[TASK5_STK_SIZE];/*逻辑任务栈空间*/

volatile AppInfo mdAppInfo;


void SysVersionInfoShow()
{
	printf("########################欢迎使用物联网快速接入开发平台########################\n");
	printf("#SoftVersion:%s\n",SYS_SOFT_VERSION);
	printf("#HadVersion:%s\n",SYS_HAD_VERSION);
	printf("#作者:谢桂\n");
	printf("#交流地址:1048070562\n");
	printf("#操作系统类型:UCOSIII\n");
	printf("########################欢迎使用物联网快速接入开发平台########################\n");

}

void SpearkInit()
{
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	
	GPIO_Init_Structure.GPIO_Pin = GPIO_Pin_13; 
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_Init_Structure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}
void AppInit()
{
	SpearkInit();
}
/*1、检测蓝牙模块是否有要充电的设备,
如果有设备需要充电则进入充电模式，然后上传充电时间
硬件连接关系：
usart2 --- 蓝牙模块*/
static void TaskStart(void)
{
	OS_ERR 		err;
	SysVersionInfoShow();
	AppInit();
	ESP8266_Init();/*wifi模块初始化*/
	
}

/*主任务，负责创建子任务*/
void KernelMain(void)
{
    OS_ERR err;
	
    CPU_Init();
    OSInit(         (OS_ERR	    *)&err);

    OSTaskCreate(   (OS_TCB     *)&taskStartTCB,
                    (CPU_CHAR   *)"Task Start",
                    (OS_TASK_PTR)TaskStart,/*主任务*/
                    (void       *)0,
                    (OS_PRIO    ) STARTUP_TASK_PRIO,
                    (CPU_STK    *)&taskStartStk[0],
                    (CPU_STK_SIZE)STARTUP_TASK_STK_SIZE / 10,
                    (CPU_STK_SIZE)STARTUP_TASK_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);    
    
    OSStart(        (OS_ERR	    *)&err);
}





