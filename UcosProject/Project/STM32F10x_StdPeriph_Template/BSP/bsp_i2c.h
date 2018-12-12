/***********************************************************************************
 * 文 件 名   : bsp_i2c.h
 * 负 责 人   : 谢桂
 * 创建日期   : 2017年7月14日
 * 文件描述   : I2C总线描述层，其中包含模拟方式挖部模块驱动方式
 * 版权说明   : Copyright (c) 2008-2017   xx xx xx xx 技术有限公司
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#ifndef BSP_I2C_H
#define BSP_I2C_H

#include "BspConfig.h"
#include "stdio.h"
#include "os.h"
#include "Lib/Queue/Queue.h"

#define SCL_H         GPIO_SetBits(GPIOB,GPIO_Pin_6) 
#define SCL_L         GPIO_ResetBits(GPIOB,GPIO_Pin_6)  
    
#define SDA_H         GPIO_SetBits(GPIOB,GPIO_Pin_7) 
#define SDA_L         GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define SCL_STATE      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) 
#define SDA_STATE      GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7) 


int8_t SimulationI2cInit( );
void SimulationI2cStart();
void SimulationI2cStop();
void SimulationI2cSendACK(uint8_t ack);
uint8_t SimulationI2cRecvACK();
void SimulationI2cSendByte(uint8_t dat);
uint8_t Simulationl2cRecvByte();

#endif
