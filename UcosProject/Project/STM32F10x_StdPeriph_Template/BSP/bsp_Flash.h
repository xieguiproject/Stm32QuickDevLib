#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include "BspConfig.h"
#include "os.h"
#include "bsp_Timer.h"

#ifdef STM32F10X_HD

#define STM32_FLASH_SIZE 512

#elif STM32F10X_MD

#define STM32_FLASH_SIZE 256

#endif

#if STM32_FLASH_SIZE < 256

#define STM_SECTOR_SIZE 1024 

#else
#define STM_SECTOR_SIZE 2048 
#endif
#define STM32_DATA_SIZE 4096

/*4K 空间用于存放用户数据*/

#define STM32_FLASH_BASE 0x08000000
#define STM32_DATE_BASE  (STM_SECTOR_SIZE + STM32_FLASH_SIZE * 1024 - STM32_DATA_SIZE)

void FlashWrite(uint32_t uiWriteAddr,uint16_t *pBuffer,uint16_t uiNumOfWrite);
void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);
#endif
