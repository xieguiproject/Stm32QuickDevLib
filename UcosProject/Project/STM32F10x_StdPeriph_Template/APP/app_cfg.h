/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_FS_EN                          DEF_DISABLED
#define  APP_CFG_USB_DEV_EN                     DEF_DISABLED
#define  APP_CFG_USB_OTG_EN                     DEF_DISABLED
#define  APP_CFG_USB_HOST_EN                    DEF_DISABLED


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                               2

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         512

/*
*********************************************************************************************************
*                                        uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN            DEF_ENABLED
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN             DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN                   DEF_ENABLED
#define  LIB_MEM_CFG_POOL_NBR                             10
#define  LIB_MEM_CFG_HEAP_SIZE                   (2 * 1024L)

/*
*********************************************************************************************************
*                                       BSP CONFIGURATION
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL                   BSP_SER_COMM_UART_02


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                   0
#define  TRACE_LEVEL_INFO                                  1
#define  TRACE_LEVEL_DBG                                   2

#define  APP_TRACE_LEVEL                        TRACE_LEVEL_DBG
#define  APP_TRACE                              BSP_Ser_Printf

#include  <cpu.h>
void  BSP_Ser_Printf   (CPU_CHAR *format, ...);

#define  FS_TRACE_LEVEL                         TRACE_LEVEL_DBG
#define  FS_TRACE                               BSP_Ser_Printf

#define  APP_TRACE_INFO(x)                      ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                       ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG ) ? (void)(APP_TRACE x) : (void)0)




/* task priority */
#define STARTUP_TASK_PRIO 4

#define TASK1_PRIO 5
#define TASK2_PRIO 6
#define TASK3_PRIO 7
#define Disp_PRIO  15

/* task stack size */
#define STARTUP_TASK_STK_SIZE 100
#define TASK1_STK_SIZE 800
#define TASK2_STK_SIZE 0x100
#define TASK3_STK_SIZE 400
#define TASK4_STK_SIZE 800
#define TASK5_STK_SIZE 0X500

#define TASK_MINI_STK_SIZE 0X100



#endif
