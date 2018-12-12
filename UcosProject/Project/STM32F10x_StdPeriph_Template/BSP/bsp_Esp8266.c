/*介于串口驱动层与网络层的驱动层*/
#include "bsp_Esp8266.h"
#include "string.h"

Esp8266 mdEsp8266;

Esp8266ParInfo Esp8266ParmInfos[] =
{
		/*0*/{"AT\r\n","OK",1,2000,NULL},/*查找设备命令*/
	/*1*/{"AT+RST\r\n","OK",1,4000,NULL},
	/*2*/{"ATE","OK",1,1000,NULL},
	
	/*下面是WIFI相关的设置*/
	/*3*/{"AT+CWMODE=","OK",1,1000,NULL},
	/*4*/{"AT+CWLAP","OK",1,2000,NULL},/*这个要加回掉函数进行wifi解析*/
	/*5*/{"AT+CWJAP=","OK",1,8000,NULL},/*链接到制定的wifi*/
	/*6*/{"AT+CWQAP","OK",1,1000,NULL},/*断开wifi*/
	/*7*/{"AT+CIPMUX=","OK",1,3000,NULL},
	/*8*/{"AT+CIPSTART=","OK",1,2000,NULL},
	/*9*/{"AT+CIPSEND=","SEND OK",1,2000,NULL},
	/*10*/{"AT+CIPCLOSE=","OK",1,1000,NULL},
	/*11*/{"AT+CWJAP?\r\n","OK",1,4000,NULL},
	/*12*/{"AT+CIFSR\r\n","OK",1,2000,NULL},

	/*13*/{"AT+CIPSERVER=","OK",1,2000,NULL},
	/*14*/{"AT+CIPSTATUS","+CIPSTATUS:0",1,4000,NULL}
};

/*模块参数初始化*/
int8_t ESP8266_ParmInit()
{
	mdEsp8266.Usart = ESP8266_BIND_USART_PORT; /*绑定在串口2上*/
	/*对应串口的初始化*/
	USARTInit(USART3,115200,NULL);
	/*设置对应串口的接受模式为数组模式*/
	mdEsp8266.Usart->USART_RecvQueue.QueueType = QueueArryType;
	//printf("\nWIFI模块控制链路初始化成功\n");
}
/*模块初始化*/
int8_t ESP8266_Init()
{
	OS_ERR err;
	int8_t status = 0;
	
	ESP8266_ParmInit();/*参数初始化*/
	
	/*1、复位*/
	#if 1
	do
	{
		OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
		status = ESP8266_Reset();
	}while(!status);
	printf("设备重启成功\n");
        #endif
    
	/*wifi模块硬件相关初始化*/
	/*2、查看设备是否在线*/
	do
	{
		OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
		status = ESP8266_Check();
	}while(!status);
	
	printf("wifi设备在线\n");
	do
	{
	OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
	status = ESP8266_CmdDisplay(1);
	}while(!status);
	
	/*3、工作模式的设置*/ /*1、设置为多链接模式*//*2、设置为station模式*/
	do
	{
		OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
		status = ESP8266_TcpMux(1);/*多链接设置*/
	}while(!status);
	//printf("多链接模式设置成功\n");
	//do
	//{
		OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
		status = ESP8266_WifiModeSet(ESP8266_WIFI_MODE_STATION);/*wifi模块工作模式*/
	//}while(!status);
	//printf("Wifi模块工作模式为STATION\n");
	do
	{
		/*4、wifi链接设置，链接WiFi操作*/
		
		
		//status = ESP8266_WifiConnect("Xiaomi_1F14","11111111");
		status = ESP8266_WifiConnect("TP-LINK_9111","123456789");
		OSTimeDly(  (OS_TICK    )500, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		
		//status = ESP8266_WifiConnect("phone","123456788");
	}while(!status);
	printf("wifi链接成功\n");
}
/*模块发送命令并检测返回*/
int8_t ESP8266_SendCmd(char *Cmd,char *Restrict,uint16_t len,int8_t Mode,uint16_t TimeOut)
{
	return  UsartSendCmd(mdEsp8266.Usart,Cmd,Restrict,len,Mode,NULL,NULL,NULL,TimeOut);
}


/*模块在线监测*/
int8_t ESP8266_Check()
{
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = &Esp8266ParmInfos[0];
	
	return ESP8266_SendCmd(sInfo->CmdInfo,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*查找上网设备是否在线*/
}
/*模块复位*/
int8_t ESP8266_Reset()
{
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = (Esp8266ParInfo *)&Esp8266ParmInfos[1];
	
	return ESP8266_SendCmd(sInfo->CmdInfo,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*复位设备*/
	return 1;
}
int8_t ESP8266_CmdDisplay(uint8_t Mode)
{
	char CmdBuf[10] = {0};
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = &Esp8266ParmInfos[2];
	
	sprintf(CmdBuf,"%s%d\r\n",sInfo->CmdInfo,Mode);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*回显示*/
	
}
/*下面是和wifi相关的指令*/
int8_t ESP8266_WifiModeSet(uint8_t Mode)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[30] = {0};
	
	sInfo = &Esp8266ParmInfos[3];
	
	sprintf(CmdBuf,"%s%d\r\n",sInfo->CmdInfo,Mode);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0 ,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi模式设置*/
}
int8_t ESP8266_WifiConnect(char *WifiSsid,char *WifiPasswd)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[50] = {0};
	
	sInfo = &Esp8266ParmInfos[5];
	
	sprintf(CmdBuf,"%s\"%s\",\"%s\"\r\n",sInfo->CmdInfo,WifiSsid,WifiPasswd);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi模式设置*/
}
/*wifi状态查询*/
int8_t ESP8266_WifiConnectStatus()
{
	
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = &Esp8266ParmInfos[11];
	
	return  ESP8266_SendCmd(sInfo->CmdInfo,sInfo->CmdRsp,0 ,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi模式设置*/
}
/*断开wifi*/
int8_t ESP8266_WifiDisconnect()
{
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = &Esp8266ParmInfos[6];
	
	return  ESP8266_SendCmd(sInfo->CmdInfo,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi模式设置*/
}

/*多链接设置*/
int8_t ESP8266_TcpMux(int8_t Mode)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[30] = {0};
	
	sInfo = &Esp8266ParmInfos[7];
	
	sprintf(CmdBuf,"%s%d\r\n",sInfo->CmdInfo,Mode);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp, 0,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi多链接设置*/
	
}
/*tcp链接*/
int8_t ESP8266_TcpClientConnect(uint8_t Socket,char *ServerIp,uint16_t ServerPort)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[50] = {0};
	
	sInfo = &Esp8266ParmInfos[8];
	
	sprintf(CmdBuf,"%s%d,\"TCP\",\"%s\",%d\r\n",sInfo->CmdInfo,Socket,ServerIp,ServerPort);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0 ,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi多链接设置*/
}
/*检测socket链接状态*/
int8_t ESP8266_TcpConnectStatus()
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[20] = {0};
	
	sInfo = &Esp8266ParmInfos[14];
	
	sprintf(CmdBuf,"%s\r\n",sInfo->CmdInfo);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0 ,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi多链接设置*/
}
/*查询本地IP地址*/
int8_t ESP8266_TcpLocalIp()
{
	Esp8266ParInfo *sInfo = NULL;
	
	sInfo = &Esp8266ParmInfos[12];
	
	return  ESP8266_SendCmd(sInfo->CmdInfo,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi模式设置*/
}
int8_t ESP8266_TcpSend(uint8_t Id,char *Buf,uint16_t Len)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[512] = {0};
	OS_ERR err;
	int8_t status = 0;
	
	sInfo = &Esp8266ParmInfos[9];
	
	sprintf(CmdBuf,"%s%d,%d\r\n",sInfo->CmdInfo,Id,Len);
	status = ESP8266_SendCmd(CmdBuf,">", 0,sInfo->CmdMode,
			sInfo->CmdTimeOut);/*wifi多链接设置*/
	
	QueueSetEmpty(&mdEsp8266.Usart->USART_RecvQueue);/*缓存队列*/
	
	status = ESP8266_SendCmd(Buf,sInfo->CmdRsp, Len,sInfo->CmdMode,
			sInfo->CmdTimeOut);/*wifi多链接设置*/;

}
/*断开TCP链接*/
int8_t ESP8266_TcpClientDisconnect(uint8_t Id)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[50] = {0};
	
	sInfo = &Esp8266ParmInfos[10];
	
	sprintf(CmdBuf,"%s%d\r\n",sInfo->CmdInfo,Id);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0 ,sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi多链接设置*/
}
/*下面是TCP服务器相关操作0,关闭1开启*/
int8_t ESP8266_TcpServerConfig(uint8_t SwitchMode,uint16_t ServerPort)
{
	Esp8266ParInfo *sInfo = NULL;
	char CmdBuf[50] = {0};
	
	sInfo = &Esp8266ParmInfos[13];
	
	sprintf(CmdBuf,"%s%d,%d\r\n",sInfo->CmdInfo,SwitchMode,ServerPort);
	
	return  ESP8266_SendCmd(CmdBuf,sInfo->CmdRsp,0, sInfo->CmdMode,
				sInfo->CmdTimeOut);/*wifi多链接设置*/
}

/*****************************************************************************
 * 函 数 名  : ESP8266_GetTimeFromStr
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月5日
 * 函数功能  : 从网络返回的字符串中提取出时间信息
 * 输入参数  : struct tm *localTime  本地时间指针
               uint8_t *DataPoint  字符串指针
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t ESP8266_GetTimeFromStr( struct tm *localTime, uint8_t *DataPoint )
{
    /*57939 17-07-05 13:03:47 50 0 0   0.0 UTC(NIST) * */
    uint8_t *Str = NULL;
    char time[3] = {0, 0, 0};								//时间缓存区
    Str = strstr(DataPoint," ");
    if(NULL != Str)
    {
	Str++;
	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_year = atoi(time);
	/*年*/
	Str+=3;

	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_mon = atoi(time);
	/*月*/
	Str+=3;

	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_mday = atoi(time);
	/*日*/
	Str+=3;


	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_hour = atoi(time);
	/*时*/
	Str+=3;

	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_min = atoi(time);
	/*分*/
	Str+=3;

	time[0] = Str[0];
	time[1] = Str[1];
	localTime->tm_sec = atoi(time);
	/*秒*/
	localTime->tm_isdst = 1;
	
    }
    
    
}
/*****************************************************************************
 * 函 数 名  : ESP8266_GetTimeFromNet
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月5日
 * 函数功能  : 从网络中获取时间信息
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : int8_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t ESP8266_GetTimeFromNet( void )
{
   OS_ERR err;
    int8_t ConnectStatus = 0;
    uint8_t *TimeStr = NULL;
    struct tm localTime;
    uint16_t  Len = 0;
    uint8_t buf[512] = 0;
	
    /*1、链接到授时中心*/
    ESP8266_TcpMux(1);/*设置多路链接*/

   do
   {
	    ConnectStatus = ESP8266_TcpClientConnect(0,"24.56.178.140",13);
	    if(ConnectStatus)
	    {
			printf("授时中心链接成功\n");
	    }
    }
    while(!ConnectStatus);
    
    if(-1 != (Len =  UsartWaitRecviceData(mdEsp8266.Usart,0,Len,buf,2000)))
    {
			ESP8266_GetTimeFromStr(&localTime,TimeStr);
			OSTimeDly(  (OS_TICK    )2000, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);

    	ESP8266_TcpClientDisconnect(0);
    }    
}