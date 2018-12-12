#include "bsp_Lcd1602.h"

void  LCD_Wait(void)	//LCD忙判断
{
	GPIO_ResetBits(LCD_RS_PORT,LCD_RS_PIN);
	GPIO_SetBits(LCD_RW_PORT,LCD_RW_PIN);
	GPIO_SetBits(LCD_REN_PORT,LCD_REN_PIN);
	
	GPIO_ResetBits(LCD_REN_PORT,LCD_REN_PIN);		
} 

void LCD_Write(unsigned char style, unsigned char input)	 //1数据，0命令
{		
	uint16_t uiOldPortData = 0;
	if(style)
	{
		GPIO_SetBits(LCD_RS_PORT,LCD_RS_PIN);
	}
	else
	{
		GPIO_ResetBits(LCD_RS_PORT,LCD_RS_PIN);
	}
	GPIO_ResetBits(LCD_RW_PORT,LCD_RW_PIN);
	
	uiOldPortData = GPIO_ReadOutputData(LCD_DATA_PORT);
	uiOldPortData = ((uiOldPortData & 0xff00)| (input & 0xff));
	
	GPIO_Write(LCD_DATA_PORT,uiOldPortData);
	
	GPIO_ResetBits(LCD_REN_PORT,LCD_REN_PIN);		
	//delay(15);
	GPIO_SetBits(LCD_REN_PORT,LCD_REN_PIN);		
	//delay(15);
	GPIO_ResetBits(LCD_REN_PORT,LCD_REN_PIN);		
	LCD_Wait();	
}

void LCD_Initial(void)			  //LCD液晶初始化
{
	/*端口初始化*/
	
	GPIO_InitTypeDef GPIO_Init_Structure;
	
	GPIO_Init_Structure.GPIO_Pin = GPIO_Pin_All; 
	GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(LCD_DATA_PORT,&GPIO_Init_Structure);
	
	LCD_Write(0,0x38); 
	//delay(300);
	LCD_Write(0,0x0c);           
	LCD_Write(0,0x01);        
	LCD_Write(0,0x06);     
}

void GotoXY(unsigned char x, unsigned char y)
{
	if(y==0)	
		LCD_Write(0,0x80|x);
	else if(y==1)    
		LCD_Write(0,0xc0|x);
}

void ShowString(unsigned char *str)
{
	LCD_Write(0,0x0c); 
	while(*str)  LCD_Write(1,*str++);
}

