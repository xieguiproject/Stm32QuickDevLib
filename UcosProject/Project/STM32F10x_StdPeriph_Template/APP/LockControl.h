#ifndef _LOCK_CONTROL_H
#define _LOCK_CONTROL_H

#include "bsp.h"


#define FIND_BOARD_ADDR_HEAD 						0X81		/*板卡地址查询*/
#define OPEN_BOARD_LOCK_HEAD						0X8A		/*开锁*/
#define READ_BOARD_LOCK_HEAD 						0X80		/*读取锁状态*/
#define READ_BOARD_PROTCOL_VERSION_HEAD             0X90		/*读取控制协议*/

#undef CONTROL_LOCK_BORAD_RSP_TYPE                                     /*该宏定义表示开锁后是否1s会返回数据*/

#define LOCK_CONTROL_USART                                               (&mdUsart.USART3_s)		/*锁控制器绑定串口*/

#define  BOARD_NUM										2				/*总共两张板卡*/
#define  LOCK_MSG_NUM                                                          15
#define  LOCK_MSG_FREE                                                          0X00
#define  LOCK_MSG_USE                                                           0X01
#define   BOARD_LOCK_NUM_24 						        25				/*24口*/
#define  BOARD_LOCK_NUM_50							 51				/*50口*/
#define  READ_LOCK_STAT_INT							200						/*读取锁状态间隔时间*/
#define BOARD_LOCK_NUMS 								BOARD_LOCK_NUM_50   

#define BOARD_LOCK_OPEN_STATE           			               0X11		/*锁为开启状态*/
#define BOARD_LOCK_CLOSE_STATE 					        0X22		/*锁为关闭状态*/


typedef struct tagBoardInfo_s
{
	uint8_t ucBoardAddr;	/**对应板卡的地址*/
	uint8_t ucBoardLockState[BOARD_LOCK_NUMS];/*该板卡上所有的锁状态*/
	uint8_t ucBoardUsed;													/*è?1?2é?ˉμ?°??¨′??ú?òê1ó???μ??·*/
}BoardInfo;


typedef struct{  
    uint8_t bit0:1;  
    uint8_t bit1:1;  
    uint8_t bit2:1;  
    uint8_t bit3:1;  
    uint8_t bit4:1;  
    uint8_t bit5:1;  
    uint8_t bit6:1;  
    uint8_t bit7:1;  
}bitss;

typedef union{  
    uint8_t data_char;  
    bitss  data_bits;   
}utype;


/*查询板卡地址*/
typedef struct tagFindBoardAddr_s
{
	uint8_t ucHead;			/*协议头部*/
	uint8_t ucBoardAddr;		/*板卡地址*/
	uint8_t ucStatic;			/*固定值*/
	uint8_t ucState;			/*状态*/
	uint8_t ucCrc;				/*crc校验*/
}FindBoardAddr;

/*查询板卡地址响应帧*/
typedef struct tagFindBoardAddrRsp_s
{
	uint8_t ucHead;			/*头部*/
	uint8_t ucBoardAddr;		/*板卡地址*/
	uint8_t ucStatic;			/*固定值*/
	uint8_t ucState;			/*状态*/
	uint8_t ucCrc;				/*crc校验*/
}FindBoardAddrRsp;

/*开锁指令*/
typedef struct tagOpenBoardLock_s
{
	uint8_t ucHead;			/*头部*/
	uint8_t ucBoardAddr;		/*板卡地址*/
	uint8_t ucLockAddr;		 /*锁地址*/
	uint8_t ucState;			/*状态*/
	uint8_t ucCrc;				/*crc校验*/
}OpenBoardLock;

/*开锁指令返回*/
typedef struct tagOpenBoardLockRsp_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;			/*板卡地址*/
	uint8_t ucLockAddr;		    /*锁地址*/
	uint8_t ucState;					/*状态*/
	uint8_t ucCrc;						/*crc校验*/
}OpenBoardLockRsp;

/*读取锁状态*/
typedef struct tagReadBoardLockState_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;			/*板卡地址*/
	uint8_t ucLockAddr;		      /*锁地址*/
	uint8_t ucCmd;				/*状态*/
	uint8_t ucCrc;					/*crc校验*/
}ReadBoardLockState;


#if BOARD_LOCK_NUMS == BOARD_LOCK_NUM_24

typedef struct tagReadBoardLockStateRsp_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;		 	/*板卡地址*/
	uint8_t ucState1;
	uint8_t ucState2;
	uint8_t ucState3;
	uint8_t ucCmd;				/*命令*/
	uint8_t ucCrc;				     /*crc*/
}ReadBoardLockStateRsp;
#else
typedef struct tagReadBoardLockStateRsp_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;			/*板卡地址*/
	uint8_t ucState1;
	uint8_t ucState2;
	uint8_t ucState3;
	uint8_t ucState4;
	uint8_t ucState5;
	uint8_t ucState6;
	uint8_t ucState7;
	uint8_t ucCmd;				/*命令*/
	uint8_t ucCrc;				/*crc*/
}ReadBoardLockStateRsp;	
#endif
/*读取协议版本*/
typedef struct tagReadProtcolVersion_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;			/*板卡地址*/
	uint8_t ucCmd1;
	uint8_t ucCmd2;
	uint8_t ucCrc;					/*crc校验*/
}ReadProtcolVersion;

/*读取协议版本返回*/
typedef struct tagReadProtcolVersionRsp_s
{
	uint8_t ucHead;				/*头部*/
	uint8_t ucBoardAddr;		/*板卡地址*/
	uint8_t ucCmd1;
	uint8_t ucVersion;
	uint8_t ucCrc;				/*crc校验*/
}ReadProtcolVersionRsp;

typedef struct tagLockMsgType
{
    uint8_t cmd;    /*命令*/
    uint8_t arg1;   /*参数1*/
    uint8_t arg2;   /*参数2*/
    uint8_t stau;   /*该消息是否在使用*/
}LockMsgType;

typedef struct tagLockControl
{
	USARTInfo_s * Usart;	/*这是一个串口设备，需要设置绑定的串口位置*/
	BoardInfo  sAllBoardInfo[BOARD_NUM + 1];/*所有的板状态*/
	OS_TCB     taskTCB;			/*TCB*/
 	CPU_STK    taskStk[0x100];/*发送任务占空间*/
  OS_Q         msg;/*消息队列用于开锁函数与锁控制线程消息通信*/
  LockMsgType MsgBuf[LOCK_MSG_NUM]; /*15个消息空间*/
}LockControl;



extern LockControl mdLockControl;
#ifdef __cplusplus
extern "C"
{
#endif
void ControlControlTask(void *p_arg);
uint16_t LockControlReadAllLock(uint8_t BoardAddr);
int8_t LockControlInit( void );
int8_t LockControlOpenALockMsg(uint8_t BoradAddr,uint8_t LockAddr);
int8_t LockControlOpenALock(uint8_t BoradAddr,uint8_t LockAddr);
int8_t LockShowAllLockState();
#ifdef __cplusplus
}
#endif
#endif
