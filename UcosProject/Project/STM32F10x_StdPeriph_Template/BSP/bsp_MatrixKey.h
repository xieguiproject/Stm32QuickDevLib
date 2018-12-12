#ifndef BSP_MATRIXKEY_H
#define BSP_MATRIXKEY_H

#include "stm32f10x.h"	
#include "os.h"

#define ROW0 GPIO_Pin_13
#define ROW1 GPIO_Pin_11
#define ROW01PORT GPIOB

#define ROW2 GPIO_Pin_15
#define ROW3 GPIO_Pin_13
#define ROW23PORT GPIOE

#define LOW0 GPIO_Pin_11
#define LOW1 GPIO_Pin_9
#define LOW2 GPIO_Pin_7
#define LOW012PORT GPIOE

#define LOW3 GPIO_Pin_0
#define LOW3PORT GPIOG




/*
PB13
PB11
PE15
PE11
PE13
PE9
PE7
PG0
*/
typedef struct tagMatrixIoCfg
{
	GPIO_TypeDef *GpioPort;
	uint16_t GpioPin;
}MatrixIoCfg;

void CombineGpioInit();
uint8_t  Combine8bitPortRead();
void Combine8bitPortWrite(uint8_t ucDat);
extern int16_t KeyValue,KeyFlage;
int MatrixKeyScan();

extern MatrixIoCfg MatrixIoCfgInfo[8];
#endif