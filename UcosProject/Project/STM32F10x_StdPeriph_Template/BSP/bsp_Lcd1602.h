#ifndef BSP_LCD1602_H
#define BSP_LCD1602_H

#include "BspConfig.h"
#include "os.h"
#include "bsp_Timer.h"

#define LCD_RS_PORT GPIOB
#define LCD_RS_PIN  GPIO_Pin_8

#define LCD_RW_PORT GPIOB
#define LCD_RW_PIN  GPIO_Pin_9

#define LCD_REN_PORT GPIOB
#define LCD_REN_PIN  GPIO_Pin_12

#define LCD_BUY_PORT GPIOB
#define LCD_BUY_PIN  GPIO_Pin_7

#define LCD_DATA_PORT GPIOB

void LCD_Initial(void);
void GotoXY(unsigned char x, unsigned char y);
void ShowString(unsigned char *str);

#endif
