#include "bsp_Flash.h"


uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];

/*flash操作，使用4k FLASH模拟errrom操作，其中1k用于*/

u16 FLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i] = FLASH_ReadHalfWord(ReadAddr);
		ReadAddr+=2;
	}
}
void FLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;
	}  
}
/*****************************************************************************
 * 函 数 名  : FlashWrite
 * 负 责 人  : xiegui
 * 创建日期  : 2018年1月25日
 * 函数功能  : flash 写僮
 * 输入参数  : uint32_t uiWriteAddr   写数据刂
               uint16_t *pBuffer      数据地址
               uint16_t uiNumOfWrite  长度 2纸
 * 输出参数  : 无
 * 返 回 值  : 
 * 调用关系  : 
 * 其    它  : 

*****************************************************************************/
void FlashWrite(uint32_t uiWriteAddr,uint16_t *pBuffer,uint16_t uiNumOfWrite)
{

	uint32_t uiOffAddr = 0;/*实际偏移地址*/
	uint32_t uiSecPos = 0;/*扇区地址*/
	uint16_t uiSecOff = 0;/*扇区内偏移地址*/
	uint16_t uiSecRemain = 0;/*扇区内剩余地址*/
	uint16_t uiLoop = 0;
	
	if(uiWriteAddr < STM32_FLASH_BASE || uiWriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))
	{
		return;
	}
	/**解锁flash*/
	FLASH_Unlock();
	uiOffAddr = uiWriteAddr - STM32_FLASH_BASE;
	uiSecPos = uiOffAddr / STM_SECTOR_SIZE;
	uiSecOff =  (uiOffAddr % STM_SECTOR_SIZE) / 2;
	uiSecRemain = (STM_SECTOR_SIZE /2 - uiSecOff); 


	if(uiNumOfWrite <= uiSecRemain) 
	{
		uiSecRemain = uiNumOfWrite;
	}
	while(1)
	{
		FLASH_Read(uiSecPos * STM_SECTOR_SIZE + STM32_FLASH_BASE,
						STMFLASH_BUF,STM_SECTOR_SIZE/2);
		for(uiLoop = 0; uiLoop < uiSecRemain;uiLoop++)
		{
			if(STMFLASH_BUF[uiSecOff + uiLoop] != 0xffff)
			{
				break;
			}
		}
		if(uiLoop < uiSecRemain)
		{
			FLASH_ErasePage(uiSecPos * STM_SECTOR_SIZE+STM32_FLASH_BASE);
			for(uiLoop = 0;uiLoop < uiSecRemain;uiLoop++)
			{
				STMFLASH_BUF[uiLoop + uiSecOff]=pBuffer[uiLoop];	  
			}
			FLASH_Write_NoCheck(uiSecPos * STM_SECTOR_SIZE+STM32_FLASH_BASE,
								STMFLASH_BUF,STM_SECTOR_SIZE/2);
		}
		else
		{
			FLASH_Write_NoCheck(uiOffAddr,pBuffer,uiSecRemain);
		}
		if(uiNumOfWrite == uiSecRemain)
		{
			break;
		}
		else
		{
			uiSecPos++;
			uiSecOff = 0;
			pBuffer += uiSecRemain;
			uiWriteAddr += uiSecRemain;
			uiNumOfWrite -=uiSecRemain;
			if(uiNumOfWrite > STM_SECTOR_SIZE / 2)
			{
				uiSecRemain = STM_SECTOR_SIZE / 2;
			}
			else
			{
				uiSecRemain = uiNumOfWrite;
			}
		}
	}
	FLASH_Lock();
}

