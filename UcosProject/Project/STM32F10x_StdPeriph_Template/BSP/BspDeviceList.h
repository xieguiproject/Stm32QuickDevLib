#ifndef BSP_DEVICE_LIST_H
#define BSP_DEVICE_LIST_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"



typedef struct tagDeviceOperations
{	
	/*通过设备名称在链表中对应,打开成功后返回设备文件描述符号，
	这个符号为设备在链表中的位置 + 1*/
	int (*DeviceOpen) (char *DeviceName,int *arg);
	/*打开操作*/

	/*读取操作*/
	int (*DeviceRead)(int DeviceFd,char *UserBuf,int len);
	/*写操作*/
	int (*DeviceWrite)(int DeviceFd,char *UserBuf,int len);
}DeviceOperations;/*设备操作结构体*/

/*设备详细信息*/
typedef struct tagDeviceInfo
{
	char DeviceName[10];	/*设备名称，主要是通过这个来对应设备*/
	int  DeviceId;			/*设备id号码*/
}DeviceInfo;

typedef struct tagSysDevStrutInfo
{
	DeviceOperations DevOpeatationInfo;	/*设备节点详细信息*/
	DeviceInfo       NodeInfo;			/*节点信息*/
	struct tagSysDevStrutInfo *next;	/*下一个节点*/
}SysDevStrutInfo;

typedef void (*ModeCallBack)(SysDevStrutInfo *DeviceNode);
SysDevStrutInfo * DeviceListInit( void );
void mode_init(ModeCallBack Func);

extern SysDevStrutInfo *DeviceListHead;

#endif
