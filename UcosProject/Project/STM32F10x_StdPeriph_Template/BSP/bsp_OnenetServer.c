#include "bsp_OnenetServer.h"
/*1、链接到onenet服务器*/

static OS_TCB		taskHartTCB;
static  CPU_STK     taskHartStk[TASK3_STK_SIZE];/*逻辑任务栈空间*/

/*双字节转换，将高字节转为低字节*/
uint16_t ConvDat(uint16_t dat)
{
	uint16_t temp =0;

	temp = ((dat % 256) << 8) | (dat / 256);

	return temp;
}

/*****************************************************************************
 * 函 数 名  : OnenetConnectMqttServer
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月27日
 * 函数功能  : 链接到MQTT服务器
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void OnenetConnectMqttServer(uint8_t *ucMqttServerIp,uint16_t uiMqttServerPort)
{
	OS_ERR err;
	
	int8_t status = 0;
	do
	{
		status = ESP8266_TcpClientConnect(0,ucMqttServerIp,uiMqttServerPort);
		
		OSTimeDly(  (OS_TICK    )100, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
	}while(!status);
}

void OnenetMqttConnect(uint16_t KeepOnlineTime,char *DeviceId,
							 char *UserName,char *Password)
{
	MQTTConnect MQTTConnectPar;

	char ucDeviceIdBuff[USER_BUFF_LEN] = {0};/*用户信息数组*/
	char ucUserNameBuff[USER_BUFF_LEN] = {0};/*用户信息数组*/
	char ucPasswordBuff[USER_BUFF_LEN] = {0};/*用户信息数组*/
	char MessageBuf[128] = {0};
	uint16_t MessageLen = 0;
	uint8_t DeviceIdLen = 0;	/*长度*/
	uint8_t UserNmaeLen = 0;
	uint8_t PasswordlLen =0;

	memset(&MQTTConnectPar,0,sizeof(MQTTConnect));
	
	if(DeviceId == NULL || UserName == NULL || Password == NULL)
	{
		LogMsgStr(LOG_DEBUG, "DeviceID Or UserName Or Password Is NULL\n");
		return;
	}

	/*负载*/
	DeviceIdLen = strlen(DeviceId);
	UserNmaeLen = strlen(UserName);
	PasswordlLen = strlen(Password);
	
	/*固定不变的部分*/
	MQTTConnectPar.sHeader.ucPacketType = (MQTT_PKT_CONNECT << 4);/*登录*/
	MQTTConnectPar.sHeader.ucRemainingLenth = (sizeof(MQTTConnect) - 2) + 
												DeviceIdLen +2 + UserNmaeLen +2 + 
											PasswordlLen + 2;/*余下的长度*/

	/*可变部分*/
	MQTTConnectPar.uiProtocolNameLength = ConvDat(0x04);
	MQTTConnectPar.uiMqttName[0] = 'M';
	MQTTConnectPar.uiMqttName[1] = 'Q';
	MQTTConnectPar.uiMqttName[2] = 'T';
	MQTTConnectPar.uiMqttName[3] = 'T';/*存放的是ASSIC码*/
	MQTTConnectPar.ucProtocolLevel = 0x04;
	MQTTConnectPar.ucConnectFlag |= (CONNECT_USER_FLAGE | CONNECT_PASSWD_FLAGE);/*这两个标志必须为1，否则服务器不会理你*/
	MQTTConnectPar.ucKeepAliveTime = ConvDat(KeepOnlineTime);/*保持连接时间，最大为65535约18个小时*/
	
	

	if(DeviceIdLen > USER_BUFF_LEN || UserNmaeLen > USER_BUFF_LEN || DeviceIdLen > USER_BUFF_LEN)
	{
		LogMsgStr(LOG_DEBUG, "DeviceIdLen Or UserNmaeLen Or DeviceIdLen overflow\n");
		return;/*错误退出*/
	}

	ucDeviceIdBuff[0] = 0;/*高*/
	ucDeviceIdBuff[1] = DeviceIdLen;/*低*/
	
	memcpy(&ucDeviceIdBuff[2],DeviceId,DeviceIdLen);
	
	ucUserNameBuff[0] = 0;/*高*/
	ucUserNameBuff[1] = UserNmaeLen;/*低*/
	
	memcpy(&ucUserNameBuff[2],UserName,UserNmaeLen);
	
	ucPasswordBuff[0] = 0;/*高*/
	ucPasswordBuff[1] = PasswordlLen;/*低*/
	
	memcpy(&ucPasswordBuff[2],Password,PasswordlLen);
	
	
	memcpy(MessageBuf,&MQTTConnectPar,sizeof(MQTTConnect));/*固定头部复制*/
	
	memcpy(&MessageBuf[sizeof(MQTTConnect)],ucDeviceIdBuff,DeviceIdLen + 2);/*设备信息*/
	
	memcpy(&MessageBuf[sizeof(MQTTConnect) + DeviceIdLen +2 ],ucUserNameBuff,UserNmaeLen + 2);/*用户信息*/
	
	memcpy(&MessageBuf[sizeof(MQTTConnect) + DeviceIdLen +2 + UserNmaeLen + 2 ],ucPasswordBuff,PasswordlLen + 2);/*密码信息*/
	
	
	MessageLen = sizeof(MQTTConnect) + DeviceIdLen +2 + UserNmaeLen +2 + PasswordlLen + 2;
	
	ESP8266_TcpSend(0,MessageBuf,MessageLen);
}
void OnenetKeepAliveTask(void *arg1)
{
	OS_ERR 		err;
	char MessageBuf[100] = {0};
	uint8_t MessageLen = 0;
	int8_t iStatus = 0;
	MQTTHeart Heart;
	uint16_t uiCheckWifiCount = 0;
	uint16_t uiCheckSocketCount = 0;
	uint16_t uiKeepAliveCount = 0;
	memset(&Heart,0,sizeof(MQTTHeart));
	
	Heart.sHeader.ucPacketType = (MQTT_PKT_PINGREQ << 4);
	Heart.sHeader.ucRemainingLenth = 0;
	
	memcpy(MessageBuf,&Heart,sizeof(MQTTHeart));

	MessageLen = sizeof(MQTTHeart);

	while(1)
	{
		if(++uiCheckWifiCount > 60)/*每10秒钟检查一下wifi链接状态*/
		{
			uiCheckWifiCount = 0;
			iStatus = ESP8266_WifiConnectStatus();
			if(!iStatus)/*wifi链接失败，重新链接wifi*/
			{
				do
				{
					/*4、wifi链接设置，链接WiFi操作*/
					iStatus = ESP8266_WifiConnect("Xiaomi_1F14","11111111");
					OSTimeDly(  (OS_TICK    )100, 
			                    (OS_OPT     )OS_OPT_TIME_DLY, 
			                    (OS_ERR     *)&err);
				}while(!iStatus);
			}
		}
		//检查服务器链接情况
		if(++uiCheckSocketCount > 40)
		{
			uiCheckSocketCount = 0;
			iStatus = ESP8266_TcpConnectStatus();
			if(!iStatus)
			{
				printf("Socket链接失败\r\n");
				do
				{
					iStatus = ESP8266_TcpClientConnect(0,MQTT_SERVER_IP,MQTT_SERVER_PORT);
					OSTimeDly(  (OS_TICK    )100, 
			                    (OS_OPT     )OS_OPT_TIME_DLY, 
			                    (OS_ERR     *)&err);
					
				}while(!iStatus);
				/**重连MQTT服务器*/
				OnenetMqttConnect(120,"24958983","119154","20180126172201");
			}
		}
		if(++uiKeepAliveCount > 25)
		{
			uiKeepAliveCount = 0;
			ESP8266_TcpSend(0,MessageBuf,MessageLen);
		}
		OSTimeDly(  (OS_TICK    )1000, 
	                    (OS_OPT     )OS_OPT_TIME_DLY, 
	                    (OS_ERR     *)&err);/*5000ms*/
		
		
	}
}

/*****************************************************************************
 * 函 数 名  : OnenetKeepAlive
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月27日
 * 函数功能  : 设置设备保持在线
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void OnenetKeepAliveInit()
{
	OS_ERR 		err;
#if 1	
	OSTaskCreate(   (OS_TCB     *)&taskHartTCB,
                    (CPU_CHAR   *)"taskKeepAlive",
                    (OS_TASK_PTR)OnenetKeepAliveTask,
                    (void       *)0,
                    (OS_PRIO    ) 10,
                    (CPU_STK    *)&taskHartStk[0],
                    (CPU_STK_SIZE)TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE)TASK3_STK_SIZE,
                    (OS_MSG_QTY )0,
                    (OS_TICK    )0,
                    (void       *)0,
                    (OS_OPT     )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), 
                    (OS_ERR     *)&err);
#endif
}

uint16_t CreatePublistJsonStr(char *SendBuf,float fCenterCm,
									float fHightCm,float fLowCm,
									uint16_t uiSysStatusCode,uint16_t uiWaterPercent,uint16_t *len)
{
#if 1
	sprintf(SendBuf,"{\"datastreams\":[{\"id\":\"CenterWaterLevel\",\"datapoints\":[{\"value\":%.2f}]}\
,{\"id\":\"HightWaterLevel\",\"datapoints\":[{\"value\":%.2f}]}\
,{\"id\":\"LowWaterLevel\",\"datapoints\":[{\"value\":%.2f}]}\
,{\"id\":\"SystemStatusCode\",\"datapoints\":[{\"value\":%d}]}\
,{\"id\":\"WaterPercent\",\"datapoints\":[{\"value\":%.2d}]}]}",fCenterCm,fHightCm,fLowCm,uiSysStatusCode,uiWaterPercent);
#else
	sprintf(SendBuf,"{\"datastreams\":[{\"id\":\"CenterWaterLevel\",\"datapoints\":[{\"value\":2}]},{\"id\":\"HightWaterLevel\",\"datapoints\":[{\"value\":2}]},{\"id\":\"LowWaterLevel\",\"datapoints\":[{\"value\":2}]},{\"id\":\"SystemStatusCode\",\"datapoints\":[{\"value\":3}]},{\"id\":\"WaterPercent\",\"datapoints\":[{\"value\":3}]}]}");
#endif
	*len = strlen(SendBuf);
	return 1;
}

uint16_t MQTTPublist(char *TopicName,float fCenterCm,float fHightCm,float fLowCm,
									uint16_t uiSysStatusCode,uint16_t uiWaterPercent)
{
	
	uint16_t uiLoop = 0;
	char MessageBuf[1024] = {0};
	uint16_t MessageLen = 0;
	MQTTPush MQTTPushStr;
	char TopicNameBuf[USER_BUFF_LEN] = {0};
	uint16_t TopicNameLen =0;
	uint16_t JsonLen=0;
	uint16_t uiRemainingLenth = 0;
	TopicNameLen = strlen(TopicName);
	
	memset(&MQTTPushStr,0,sizeof(MQTTPush));
	
	MQTTPushStr.sHeader.ucPacketType |= (MQTT_PKT_PUBLISH << 4);

	
	CreatePublistJsonStr(&MessageBuf[sizeof(MQTTPush) + 2 + TopicNameLen + 3],
						fCenterCm,fHightCm,fLowCm,uiSysStatusCode,uiWaterPercent,&JsonLen);

	uiRemainingLenth = 2 + TopicNameLen + 3 + JsonLen;

	/*mqtt服务器的垃圾长度计算，这算一个bug*/
	if(uiRemainingLenth >= 128)
	{
		MQTTPushStr.ucRemainingLenth2 = (uiRemainingLenth / 128);
		MQTTPushStr.sHeader.ucRemainingLenth = 128 + uiRemainingLenth % 128;
	}
	else
	{
		MQTTPushStr.sHeader.ucRemainingLenth = uiRemainingLenth;
	}
	 

	
	TopicNameBuf[0] = 0;
	TopicNameBuf[1] = TopicNameLen;

	
	memcpy(&TopicNameBuf[2],TopicName,TopicNameLen);/*长度信息*/
	
	memcpy(MessageBuf,&MQTTPushStr,sizeof(MQTTPush));/*保存开始头部*/
	
	memcpy(&MessageBuf[sizeof(MQTTPush)],TopicNameBuf,TopicNameLen + 2);/*拷贝数据*/
	
	//memcpy(&MessageBuf[sizeof(MQTTPush)],TopicNameBuf,TopicNameLen + 2);/*负载*/
	MessageBuf[sizeof(MQTTPush) + TopicNameLen + 2] = 0x01; /*数据类型1*/
	
	MessageBuf[sizeof(MQTTPush) + TopicNameLen + 3] = JsonLen /256;
	MessageBuf[sizeof(MQTTPush) + TopicNameLen + 4] = JsonLen %256;
	
	MessageLen = (sizeof(MQTTPush) + 2 + TopicNameLen + JsonLen + 3);
	
	ESP8266_TcpSend(0,MessageBuf,MessageLen);
	return 1;
}

