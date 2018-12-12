/***********************************************************************************
 * 文 件 名   : BspDeviceList.c
 * 负 责 人   : Gavin
 * 创建日期   : 2017年6月7日
 * 文件描述   : 设备链表实现，一般情况下不需要修改改文件
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/

#include "BspDeviceList.h"

/*设备全局链表*/
SysDevStrutInfo *DeviceListHead = NULL;

/*****************************************************************************
 * 函 数 名  : DeviceListInit
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月19日
 * 函数功能  : 设备链表初始化，所有的设备采用注册的方式注册到系统中，统一进
               行管理
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : 返回链表节点
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
SysDevStrutInfo * DeviceListInit( void )
{
    SysDevStrutInfo *jDeviceListHead = NULL;

	jDeviceListHead = (SysDevStrutInfo *)malloc(sizeof(SysDevStrutInfo));
	if(NULL == jDeviceListHead )
	{
		return NULL;
	}

	jDeviceListHead->next = NULL;

	return jDeviceListHead;
}

/*****************************************************************************
 * 函 数 名  : FindSelectNode
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月19日
 * 函数功能  : 根据输入的节点好找到对应的链表节点
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
SysDevStrutInfo *FindSelectNode( int NodeId)
{
	SysDevStrutInfo *Node = DeviceListHead;
	
	if(Node == NULL)
	{
		return NULL;
	}
	
	if( NodeId <= 0 )
	{
		return NULL;
	}

	while(Node != NULL)
	{
		if(Node->NodeInfo.DeviceId == NodeId)
		{
			return Node;
		}
		Node = Node->next;
	}

	return NULL;
}

/*****************************************************************************
 * 函 数 名  : FindEndNode
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月23日
 * 函数功能  : 查找链表中最后一个节点
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
SysDevStrutInfo* FindEndNode()
{
    SysDevStrutInfo *Node = DeviceListHead;

	while(Node->next != NULL)
	{
		Node = Node->next;
	}
	return Node;
	/*返回最后一个节点*/
}

/*****************************************************************************
 * 函 数 名  : FindSelectNodeFromName
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月23日
 * 函数功能  : 根据名称查找链表中的设备
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
SysDevStrutInfo* FindSelectNodeFromName( char *DeviceName)
{
	SysDevStrutInfo *Node = DeviceListHead;
	
	if(Node == NULL)
	{
		return NULL;
	}

	while(Node != NULL)
	{
		if(NULL != strstr(Node->NodeInfo.DeviceName,DeviceName))
		{
			return Node;
		}
		Node = Node->next;
	}

	return NULL;
}

/*****************************************************************************
 * 函 数 名  : device_open
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月19日
 * 函数功能  : 设备打开操作
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int  device_open(char *DeviceName,int *arg)
{
	SysDevStrutInfo *Node = NULL;
    /*1、通过设备名称在链表中找到对应的驱动*/
	/*2、回调对应的打开函数*/
	Node = FindSelectNodeFromName(DeviceName);
	Node->DevOpeatationInfo.DeviceOpen(DeviceName,arg);
}

/*****************************************************************************
 * 函 数 名  : device_write
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月23日
 * 函数功能  : 设备写函数
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int device_write(int DeviceFd,char *UserBuf,int len)
{
    SysDevStrutInfo *Node = NULL;
    /*通过设备fd找打对应的设备，并回调读取函数*/
	if(DeviceFd > 0)
	{
		Node = FindSelectNode(DeviceFd);
		return Node->DevOpeatationInfo.DeviceWrite(DeviceFd,UserBuf,len);
	}

	return -1;
}

/*****************************************************************************
 * 函 数 名  : device_read
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月19日
 * 函数功能  : 设备读取函数
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : int
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int device_read(int DeviceFd,char *UserBuf,int len)
{
	SysDevStrutInfo *Node = NULL;
    /*通过设备fd找打对应的设备，并回调读取函数*/
	if(DeviceFd > 0)
	{
		Node = FindSelectNode(DeviceFd);
		return Node->DevOpeatationInfo.DeviceRead(DeviceFd,UserBuf,len);
	}

	return -1;
}
/*驱动加载函数*/
void mode_init(ModeCallBack Func)
{
	/*分配设备id*/
	SysDevStrutInfo *Node = NULL;
	SysDevStrutInfo *NodeNew = NULL;
	
	Node = FindEndNode();

	NodeNew = (SysDevStrutInfo *)malloc(sizeof(SysDevStrutInfo));
	if(NULL == NodeNew )
	{
		while(1);
	}
	Node->next = NodeNew;

	if(NULL == Func)
	{
		while(1);
	}

	Func(NodeNew);/*调用用户模块初始化*/
	
	/*1、将传入的节点加入链表*/
}
/*模块退出函数*/
void mode_exit(ModeCallBack Func)
{
	/*找到该节点*/
	/*从链表中删除该节点*/
	
}

