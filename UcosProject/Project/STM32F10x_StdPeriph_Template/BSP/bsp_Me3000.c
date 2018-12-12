/***********************************************************************************
 * 文 件 名   : bsp_Me3000.c
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月17日
 * 文件描述   : 描述ME3000模块的程序
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "bsp_Me3000.h"

Me3000 mdMe3000;
/*#####################控制命令描述#######################*/
AtCmdParInfo AtMe3000Cmd[] =
{
        /*0*/{"AT\r\n","OK",1,1000,NULL},/*模块在线状态确认*/
        /*1*/{"AT+ZPWROFF\r\n","OK",1,1000,NULL},/*关机*/
        /*2*/{"AT+CGMR=?\r\n","+CGMR",1,1000,Me3000ReadModeVersionCallBack},/*读取版本号码*/
        /*3*/{"AT*TSIMINS?\r\n","*TSIMINS:0,1",1,1000,NULL},/*查询卡状态，无卡返回0*/
        /*4*/{"AT+CSQ\r\n","+CSQ:",1,2000,Me3000ReadSingalValueCallBack},
        /*5*/{"ATE0\r\n","OK",1,2000,NULL},/*设置命令不回显*/
        /*6*/{"AT+CNUM\r\n" ,"+CNUM:",1,1000,Me3000GetPhoneNumberCallBack},
        /*7*/{"AT+ZDNSGETIP=","ERROR",1,1000,Me3000GetHostByNameCallBack},
};

/*#####################控制命令描述#######################*/


/*gsm模块io口高低电平控制*/
uint16_t GsmGpioControl(GPIO_TypeDef* GPIO,uint16_t GPIO_Pin,uint8_t State)
{
	if(State)
	{
		GPIO_SetBits(GPIO,GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(GPIO,GPIO_Pin);
	}
}


int8_t Me3000OtherGpioInit()
{
    /*GSM模块其他GPIO初始化*/
    
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GSM_ON;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GSM_ON_PORT,&GPIO_InitStructure);
	
	GsmGpioControl(GSM_ON_PORT,GSM_ON,1);              
	 /*开机，一上电就处于开机状态*/
     
	GPIO_InitStructure.GPIO_Pin = GSM_REST;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GSM_REST_PORT,&GPIO_InitStructure);
	
	GsmGpioControl(GSM_REST_PORT,GSM_REST,0);       /*复位拉高*/
}

/*****************************************************************************
 * 函 数 名  : Me3000ParmInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : me3000硬件参数初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000ParmInit()
{
    mdMe3000.Usart = ME3000_BIND_USART_PORT;/*GSM模块绑定端口*/

    USARTInit(USART2,115200,NULL);/*初始化串口这里估计要注册回掉函数*/
	
    mdMe3000.Usart->USART_RecvQueue.QueueType = QueueArryType;
    printf("\nMe3000模块端口初始化成功\n");
    Me3000OtherGpioInit();
    printf("\nMe3000模块开机和复位端口初始化成功\n");
		Me3000TaskInit();
}

int8_t Me3000ControlTask()
{
    OS_ERR err;
    uint16_t uiLoop = 0;
    printf("\nME3000管理任务创建成功\n");
    /*主要负责让设备上线和收发数据*/
    /*1、等待3S然后判断判断设备是否在线*/
    
    OSTimeDly(  (OS_TICK    )4000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);  
        
    mdMe3000.Me3000ReadyOk = 1;/*模块工作正常*/

    Me3000SetShowMode();
    while(1)
    {
        OSTimeDly(  (OS_TICK    )5000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);
				LedClose(LED2);
        if(Me3000CheckOnline())
        {
						LedOpen(LED3);
            mdMe3000.Me3000Online = 1;
        }
        else
        {
            mdMe3000.Me3000Online = 0;      /*模块掉线*/
            mdMe3000.Me3000ReadyOk = 0;     /*模块工作错误*/
        }
        OSTimeDly(  (OS_TICK    )5000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);
        LedClose(LED3);
        /*信号强度、模块状态进行统计*/
        OSTimeDly(  (OS_TICK    )5000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);
        /*信号量读取*/
				
        Me3000ReadSingalValue();
        //Me3000GetHostByName("www.baidu.com",NULL);
    }
}

/*****************************************************************************
 * 函 数 名  : Me3000TaskInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : ME3000维护线程初始化
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000TaskInit()
{
    OS_ERR err;

	OSTaskCreate(   (OS_TCB     *)&mdMe3000.taskMe3000TCB,
                    (CPU_CHAR   *)"Me3000Task",
                    (OS_TASK_PTR)Me3000ControlTask,
                    (void       *)0,
                    (OS_PRIO    )TASK3_PRIO,
                    (CPU_STK    *)&mdMe3000.taskMe3000Stk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);
}

/*****************************************************************************
 * 函 数 名  : ME3000Init
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : ME3000模块初始化，主要是端口配置
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000Init()
{
      OS_ERR err;
      
       Me3000ParmInit();   /*参数初始化*/
       
       Me3000TaskInit();  /*管理任务创建*/
}

/*****************************************************************************
 * 函 数 名  : Me3000CheckOnline
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : 检测模块是否在线
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000CheckOnline()
{
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[0];
    
    return UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
}

/*****************************************************************************
 * 函 数 名  : Me3000Open
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : Me300模块开机操作
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000Open()
{
		OS_ERR err;
    /*1、首先判断是否已经开机*/
    if(Me3000CheckOnline())
   {
        mdMe3000.Me3000Online = 1;/*模块已经正常开机*/
        printf("\n模块已经是开机状态\n");
        return 1;
    }
    else
    {
        mdMe3000.Me3000Online = 0;/*重新开机*/
    }
    
    do
    {
        GsmGpioControl(GSM_ON_PORT,GSM_ON,0);       /*拉高*/
        OSTimeDly(  (OS_TICK    )500, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);
        GsmGpioControl(GSM_ON_PORT,GSM_ON,1);       /*拉低开机*/ 
        OSTimeDly(  (OS_TICK    )5000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);
        //GsmGpioControl(GSM_ON_PORT,GSM_ON,0);       /*拉高*/
        
    }
    while(!Me3000CheckOnline());
    
     return 1;/*终于开机成功*/
}

/*****************************************************************************
 * 函 数 名  : Me3000SoftClose
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : 通过at指令来进行关机操作
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000SoftClose()
{
		OS_ERR err;
    int8_t State = 0;
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[1];
    
   State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,NULL,NULL,NULL,cmdInfo->CmdTimeOut);

    OSTimeDly(  (OS_TICK    )1000, (OS_OPT     )OS_OPT_TIME_DLY,  (OS_ERR     *)&err);

    
   State = Me3000CheckOnline();/*检查是否关机成功*/
   if(State)
   {
        mdMe3000.Me3000Online = 0;/*模块关机*/
        return 1;
   }
   else
   {
        return 0;
   }
    
}

void *Me3000ReadModeVersionCallBack(void *arg1,void *arg2,void *arg3)
{
    /*版本号码*/
    char *Pointe = (char *)arg1;
    char *VersionPointe = NULL;
    char Version[10];
    
    if((VersionPointe = strstr(Pointe,"MG2639")) != NULL)
    {
        memcpy(Version,VersionPointe,10);
        printf("\nGSM版本:%s\n",Version);
        return VersionPointe;
    }
    else if((VersionPointe = strstr(Pointe,"ME3000")) != NULL)
    {
        memcpy(Version,VersionPointe,10);
        printf("\nGSM版本:%s\n",Version);
        return VersionPointe;
    }
    return NULL;
}

/*****************************************************************************
 * 函 数 名  : Me3000ReadModeVersion
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : 读取模块版本号码
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000ReadModeVersion()
{
    OS_ERR err;
    int8_t State = 0;
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[2];

    
    State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;
}
/*****************************************************************************
 * 函 数 名  : Me3000FindSimCard
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : 查询SIM卡状态
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int Me3000FindSimCard()
{
    OS_ERR err;
    int8_t State = 0;
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[3];

    
    State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;
}

void *Me3000ReadSingalValueCallBack(void *arg1,void *arg2,void *arg3)
{
    uint16_t uiKVaue;
    
    char *Pointe = (char *)arg1;
    char *SingnelPointe = NULL;
    char TempBuf[4] = {0};
    char bits = 0;
    if((SingnelPointe = strstr(Pointe,"+CSQ:")) != NULL)
    {
        memcpy(TempBuf,&SingnelPointe[6],2);
        uiKVaue = atoi(TempBuf);
        if(uiKVaue >= 0 && uiKVaue <= 99)
        {
            mdMe3000.SingnelValue = uiKVaue;
        }
        memset(TempBuf,0,sizeof(TempBuf));
				bits = (mdMe3000.SingnelValue >= 10)?2:1;
        memcpy(TempBuf,&SingnelPointe[6 + bits + 2],2);
        
        uiKVaue = atoi(TempBuf);
        if(uiKVaue >= 0 && uiKVaue <= 99)
        {
            mdMe3000.SingnelBer = uiKVaue;
        }
        printf("\n####信号量:%d,信号错误率%d####\n",mdMe3000.SingnelValue,
                  mdMe3000.SingnelBer);
    }
    
}
/*****************************************************************************
 * 函 数 名  : Me3000ReadSingalValue
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月17日
 * 函数功能  : 读取信号强度
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000ReadSingalValue()
{
    OS_ERR err;
    int8_t State = 0;
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[4];

    
    State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;
}

int8_t Me3000SetShowMode()
{
    OS_ERR err;
    int8_t State = 0;
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[5];

    
    State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;

}

/*****************************************************************************
 * 函 数 名  : Me3000GetPhoneNumberCallBack
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : 获取本机号码回掉处理函数
 * 输入参数  : void 
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void * Me3000GetPhoneNumberCallBack(void *arg1,void *arg2,void *arg3)
{
    
    return NULL;
}
/*****************************************************************************
 * 函 数 名  : Me3000GetPhoneNumber
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : void
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 无
 * 调用关系  : 
 * 其    它  :该函数需要提交设置电话号码

*****************************************************************************/
int8_t Me3000GetPhoneNumber(void)
{
    OS_ERR err;
    int8_t State = 0;
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[6];

    
    State =  UsartSendCmd(mdMe3000.Usart,cmdInfo->CmdInfo, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;
}

void *Me3000GetHostByNameCallBack(void *arg1,void *arg2,void *arg3)
{
    printf("122:%s\n",(char *)arg1);
}
/*****************************************************************************
 * 函 数 名  : Me3000GetHostByName
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月19日
 * 函数功能  : 根据域名获取Ip地址
 * 输入参数  : const char *Name  输入域名
 * 输出参数  : 无
 * 返 回 值  :      char *
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t Me3000GetHostByName( const char *Name,char *HostBuf)
{
    OS_ERR err;
    int8_t State = 0;
    char buf[50] ={0};
    
    AtCmdParInfo *cmdInfo = &AtMe3000Cmd[7];

    sprintf(buf,"%s%\"s\"",cmdInfo->CmdInfo,Name);
    
    State =  UsartSendCmd(mdMe3000.Usart,buf, cmdInfo->CmdRsp, 
                            0, cmdInfo->CmdMode,cmdInfo->CallBack,NULL,NULL,cmdInfo->CmdTimeOut);
    return State;
}
