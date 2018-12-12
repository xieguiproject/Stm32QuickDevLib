#ifndef _UPDATE_H
#define _UPDATE_H
/*主要负责软件远程升级，
使用到的文件
socket.c
bsp_esp8266.c
bsp_usart.c等*/
#include "bsp.h"

typedef struct tagCmdPaket
{
	uint16_t  uiHead;			/*协议头部，固定0x5aa5*/
	uint16_t  uiPaketCmd;		/*命令包类型*/
	uint8_t    uiDat[512];		/*负载*/
	uint16_t  uiCrc;			/*校验*/
	uint16_t  uiEnd;			/*结束包*/
}sCmdPaket;

/*心跳包*/
typedef struct tagHeartCmd
{
	uint16_t   times;				/*递增数*/
	uint16_t   DeviceId[20];		/*设备ID号码*/
	struct tm localTime;			/*本地时间戳*/
	uint8_t     SoftVersion[20];		/*软件版本*/
	uint8_t     SoftVersionLog[100];	/*版本更新日志*/
}HeartCmd;

/*更新包*/
typedef struct tagUpdateCmd
{
	uint16_t  uiTotalPaket;		/*总包数*/
	uint16_t  uiCenterPaket;		/*当前包数*/
	uint16_t  uiCenterPaketLen;	/*当前包长度*/
	uint8_t    ucData[512 - 6];
}UpdateCmd;
void UpdateTask(void *p_arg);
void UpdateTaskInit();

#endif