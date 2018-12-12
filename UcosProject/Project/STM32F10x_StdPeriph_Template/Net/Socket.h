#ifndef _SOCKET_H
#define _SOCKET_H

#include "BSP/bsp_Esp8266.h"

#define NET_LINK_OK                0x0001 /*wifi连接正常*/

#define MAX_SOCKET_FD           0X4

typedef enum 
{
	NET_HARDWARE_LEVEL_ESP8266 = 0,
	NET_HARDWARE_LEVEL_GSMA6
}NET_HARDWARE_TYPE;
/*网络硬件层驱动类型*/

#define NET_HARDWARE_LEVEL_TYPE  NET_HARDWARE_LEVEL_ESP8266


/*网络层回掉函数*/
typedef int (*SocketCallFunc)(int *arg1,int *arg2,int *arg3,int *arg4);

typedef struct ragSocketInfo
{
   /*对应的消息队列*/
   uint8_t flage;
}SocketInfo;
/*每个ESP8266模块可以创建四个socket 0 -4*/
typedef struct tagSocket_s
{
	/*上网设备类型*/
	   uint8_t age;
       uint16_t ucCenterNetState;
       SocketInfo SocketInfos[MAX_SOCKET_FD];
	   SocketCallFunc *SocketClientCreate;/*创建一个客户端*/
	   SocketCallFunc *SocketServerCreate;/*创建一个服务器*/
	   SocketCallFunc *SocketClose;/*关闭socketr*/
	   SocketCallFunc *SocketRead;/*读*/
	   SocketCallFunc *SocketWrite;/*写*/
       /*当前网络状态
       1、wifi死机
       2、控制器无响应
       3、连接断开
       4、网络连接正常*/
}Socket;/* 本地上网设备初始化*/

int8_t SocketLevelInit( void );

/**************************************************************************
网络层设计
1、硬件层:主要是由wifi模块，gsm模块等组成，该层主要完成的功能有
	1、自行维护上网功能，最大程度保障网络畅通，
	2、对外提供网络tcp/ip协议的基本接口
2、网络层：主要是实现网络连接的标准接口，主要包括一下接口
	1、write
	2、read
	3、close
	4、client创建和服务器的创建




**************************************************************************/
#endif