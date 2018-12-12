#include "bsp_MatrixKey.h"

int16_t KeyValue = 0;
int16_t KeyFlage = 0;

MatrixIoCfg MatrixIoCfgInfo[8] =
{
	/*p0-p7*/
	{GPIOD,GPIO_Pin_14},
	{GPIOD,GPIO_Pin_0},
	{GPIOE,GPIO_Pin_7},
	{GPIOE,GPIO_Pin_9},
	
	{GPIOE,GPIO_Pin_11},
	{GPIOE,GPIO_Pin_13},
	{GPIOE,GPIO_Pin_15},
	{GPIOD,GPIO_Pin_9},
		
};
/*组合端口初始化*/
void CombineGpioInit()
{
	uint8_t ucLoop = 0;
	
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	for(ucLoop = 0;ucLoop < 4;ucLoop++)/*前面4组设置为输出模式，后面4组设置为输入模式*/
	{
		GPIO_Init_Structure.GPIO_Pin = MatrixIoCfgInfo[ucLoop].GpioPin; 
		GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(MatrixIoCfgInfo[ucLoop].GpioPort,&GPIO_Init_Structure);
	}
	for(ucLoop = 4;ucLoop < 8;ucLoop++)/*前面4组设置为输出模式，后面4组设置为输入模式*/
	{
		GPIO_Init_Structure.GPIO_Pin = MatrixIoCfgInfo[ucLoop].GpioPin; 
		GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_Init(MatrixIoCfgInfo[ucLoop].GpioPort,&GPIO_Init_Structure);
	}
}
/*像51单片机那样操作一个合并的8位IO*/
void Combine8bitPortWrite(uint8_t ucDat)
{
	uint8_t ucLoop = 0;
	uint8_t ucTemp = ucDat;
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	for(ucLoop = 0;ucLoop < 4;ucLoop++)
	{
		if(ucTemp & 0x80)
		{
			
			GPIO_SetBits(MatrixIoCfgInfo[ucLoop].GpioPort,
									MatrixIoCfgInfo[ucLoop].GpioPin);
			
		}
		else
		{
			GPIO_ResetBits(MatrixIoCfgInfo[ucLoop].GpioPort,
									MatrixIoCfgInfo[ucLoop].GpioPin);
		}
		ucTemp = ucTemp << 1;
	}
}

uint8_t  Combine8bitPortRead()
{
	uint8_t ucLoop = 0;
	uint8_t ucTemp = 0; /*8个IO的状态*/
	uint8_t ucBit = 0;
	/*分别读取８个ＩＯ口并合并*/
	//GPIO_ReadOutputDataBit(ROW01PORT,ROW0);
	
	for(ucLoop = 4;ucLoop < 8;ucLoop++)
	{
		ucBit = GPIO_ReadInputDataBit(MatrixIoCfgInfo[ucLoop].GpioPort,
													MatrixIoCfgInfo[ucLoop].GpioPin);
		if(ucBit)
		{
			ucTemp |= (0x01 << (7 - ucLoop));
			/*0001*/
		}
	}
	return ucTemp;
}

int MatrixKeyScan()
{
  OS_ERR err;
	uint8_t ucRow = 0;
	uint8_t ucLow = 0;
	
	Combine8bitPortWrite(0xf0);/*让输出的4为设置为高*/
	
	if(Combine8bitPortRead()== 0)
	{
		return -1;
	}
	else
	{
		OSTimeDly(  (OS_TICK    )5, 
                    (OS_OPT     )OS_OPT_TIME_DLY, 
                    (OS_ERR     *)&err);
		if(Combine8bitPortRead()== 0)
		{
			return -1;
		}
	}
	KeyFlage = 1;
	Combine8bitPortWrite(0x10);
	switch(Combine8bitPortRead() & 0x0f)
	{
		case 0x08:KeyValue = 14;break;
		case 0x04:KeyValue = 0;break;
		case 0x02:KeyValue = 15;break;
		case 0x01:KeyValue = 13;break;
	}
	Combine8bitPortWrite(0x20);
	switch(Combine8bitPortRead() & 0x0f)
	{
		case 0x08:KeyValue = 7;break;
		case 0x04:KeyValue = 8;break;
		case 0x02:KeyValue = 9;break;
		case 0x01:KeyValue = 12;break;
	}
	Combine8bitPortWrite(0x40);
	switch(Combine8bitPortRead() & 0x0f)
	{
		case 0x08:KeyValue = 4;break;
		case 0x04:KeyValue = 5;break;
		case 0x02:KeyValue = 6;break;
		case 0x01:KeyValue = 11;break;
	}
	Combine8bitPortWrite(0x80);
	switch(Combine8bitPortRead() & 0x0f)
	{
		case 0x08:KeyValue = 1;break;
		case 0x04:KeyValue = 2;break;
		case 0x02:KeyValue = 3;break;
		case 0x01:KeyValue = 10;break;
	}
	return KeyValue;

}