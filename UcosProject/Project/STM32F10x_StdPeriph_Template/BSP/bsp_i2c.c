#include "bsp_i2c.h"

/*****************************************************************************
 * 函 数 名  : SimulationI2cInit
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : I2C总线端口初始化
 * 输入参数  :   初始化状态
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
int8_t SimulationI2cInit( )
{
	GPIO_InitTypeDef GPIO_InitStructure;

       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*****************************************************************************
 * 函 数 名  : Delay5us
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 延时5us
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void Delay5us()
{
    uint8_t n = 70;

    while (n--)
    {
        ;
    }
}

/*****************************************************************************
 * 函 数 名  : SimulationI2cStart
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 模拟I2C启动信号
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SimulationI2cStart()
{
    SDA_H;                    		
    SCL_H;                    		
    Delay5us();                 		
    SDA_L;                    		
    Delay5us();             
    SCL_L;                  
}

/*****************************************************************************
 * 函 数 名  : SimulationI2cStop
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 模拟I2C停止信号
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SimulationI2cStop()
{
    SDA_L;                    		
    SCL_H;                    	
    Delay5us();                 	
    SDA_H;                    	
    Delay5us();         
}

/*****************************************************************************
 * 函 数 名  : SimulationI2cSendACK
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 模拟发送ACK信号
 * 输入参数  : uint8_t ack  ACK信号
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SimulationI2cSendACK(uint8_t ack)
{
	if(ack)
	{
		SDA_H;                   
	}
	else
	{
		SDA_L;                    		
	}
    
        SCL_H;                    			
        Delay5us();                 		
        SCL_L;                    			
        Delay5us();                 	
}

uint8_t SimulationI2cRecvACK()
{
    uint8_t sda_temp;
    SCL_H;                    	
    Delay5us();                 		
    sda_temp=SDA_STATE;         
    SCL_L;                    			
    Delay5us();                 	

    return sda_temp;
}
/*****************************************************************************
 * 函 数 名  : SimulationlI2cSendByte
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 模拟I2C发送一个字节数据
 * 输入参数  : uint8_t dat  待发送的字节数
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void SimulationI2cSendByte(uint8_t dat)
{
    uint8_t ucLoop;

    for(ucLoop =0; ucLoop <8; ucLoop++)         	
    {
		if(dat&0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
        dat<<= 1;              		
        SCL_H;              
        Delay5us();             	
        SCL_L;                			
        Delay5us();             	
    }
    SimulationI2cRecvACK();
}
/*****************************************************************************
 * 函 数 名  : SimulationlI2cRecvByte
 * 负 责 人  : 谢桂
 * 创建日期  : 2017年7月14日
 * 函数功能  : 模拟I2C接受一个字节
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
uint8_t Simulationl2cRecvByte()
{
    uint8_t i;
    uint8_t dat = 0;

    SDA_H;                    		
    for (i=0; i<8; i++)         		
    {
        dat <<= 1;
        SCL_H;                		
        Delay5us();             		
		if(SDA_STATE)
		{
        	dat |= 0x01;            
		}
        SCL_L;               	 		
        Delay5us();             		
    }

    return dat;
}

