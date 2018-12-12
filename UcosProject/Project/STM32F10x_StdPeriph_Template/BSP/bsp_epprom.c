#include "Bsp_epprom.h"
/*****************************************************************************
 * 函 数 名  : EppromInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : Epprom初始化操作
 * 输入参数  : void  无
 * 输出参数  : 无
 * 返 回 值  : void
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void EppromInit( void )
{
    /*errom参数初始化操作*/
    SimulationI2cInit();
}

/*****************************************************************************
 * 函 数 名  : EppromWriteOneByte
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 向存储空间写一个字节数据
 * 输入参数  : uint8_t slave_addr  从机地址
               uint8_t byte_addr   存储地址
               uint8_t byte_dat    数据
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void EppromWriteOneByte(uint8_t slave_addr,uint8_t byte_addr,uint8_t byte_dat)
{
   uint16_t uiLoop = 0;
   
    SimulationI2cStart();     
    
    slave_addr &= 0xfe;
    
    SimulationI2cSendByte(slave_addr);     
    SimulationI2cSendByte(byte_addr);     	
    
    SimulationI2cSendByte(byte_dat);
    
    for(uiLoop = 0;uiLoop < 1000;uiLoop++);
    
    SimulationI2cStop();             		
}
/*****************************************************************************
 * 函 数 名  : EppromReadOneByte
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : Epprom读取一个字节
 * 输入参数  : uint8_t slave_addr  从机设备
               uint8_t byte_addr   读取地址
               uint8_t *byte_dat   读取的数据存储位置
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void EppromReadOneByte(uint8_t slave_addr,uint8_t byte_addr,uint8_t *byte_dat)
{
    SimulationI2cStart();            		
    slave_addr&=0xfe;
    
    SimulationI2cSendByte(slave_addr);     	
    SimulationI2cSendByte(byte_addr);     
    SimulationI2cStart();  
    
    slave_addr|=0x01;
    
    SimulationI2cSendByte(slave_addr);     
    
    *byte_dat = Simulationl2cRecvByte();
    
    SimulationI2cSendACK(1); 	
    
    SimulationI2cStop();             	
}


/*****************************************************************************
 * 函 数 名  : EppromReadBuf
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月4日
 * 函数功能  : epprom存储器读取指定数据到缓存区中
 * 输入参数  : uint16_t StartAddr  开始地址
               uint8_t * Buf  存储区域
               uint16_t Len  读取长度
 * 输出参数  : 无
 * 返 回 值  : int16_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int16_t EppromReadBuf( uint16_t StartAddr, uint8_t * Buf, uint16_t Len )
{
	uint16_t StartAddr_n = 0;
	uint16_t uiLoop = 0;
	uint8_t dat = 0;
	StartAddr_n = StartAddr;
	
	while(uiLoop < Len)
	{
		EppromReadOneByte(0xa0,StartAddr_n,&dat);
		Buf[uiLoop++] = dat;
		//SysDelayUs_d(1000);
		StartAddr_n++;
		dat = 0;
		
	}
    
}

/*****************************************************************************
 * 函 数 名  : EppromWriteBuf
 * 负 责 人  : Gavin
 * 创建日期  : 2017年6月4日
 * 函数功能  : 将指定存储空间的数据写到EPPROM指定地址中
 * 输入参数  : uint16_t StartAddr  开始地址
               uint8_t *Buf  内存地址
               uint16_t Len  长度信息
 * 输出参数  : 无
 * 返 回 值  : int16_t
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int16_t EppromWriteBuf( uint16_t StartAddr, uint8_t *Buf, uint16_t Len )
{
	uint16_t StartAddr_n = 0;
	uint16_t uiLoop = 0;
	uint8_t dat = 0;
	StartAddr_n = StartAddr % 2048;
	
	while(uiLoop < Len)
	{
		EppromWriteOneByte(0xa0,StartAddr_n % 2048,Buf[uiLoop++]);
		//SysDelayUs_d(10000);
		StartAddr_n++;	
	}
}

void EppromEase()
{
	uint16_t uiLoop = 0;
	
	for(uiLoop = 0;uiLoop<1;uiLoop++)
	{
		EppromWriteOneByte(0xa0,uiLoop,0);
	}
	
}


