/**
  ******************************************************************************
  * @file    HCT-IE-A 
  * @author  l
  * @version V1.0.0
  * @date    18-09-2011
  * @brief   RTC calendar
  ******************************************************************************
  */

#include "bsp_Calendar.h"

/*******************************************************************************
*Private Variables
*******************************************************************************/
vu8 GetTimeFlag , SetTimeFlag ;

vs16 SoftDelayTimer;					
RTC_Def TimeNow;

const uint8_t mDayNoLeap[13]=
{
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

const uint8_t mDayLeap[13]=
{
	0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


void RTC_TimeGet(RTC_Def *Times)
{
	GetTime(Times);
}

/*******************************************************************************
* Function Name  : RTC_ReConfig
* Description    : Reconfigure RTC and BKP, only used when detecting loss of
*                   BKP data.
* Input          : None
* Return         : None
*******************************************************************************/
static void RTC_ReConfig(void)
{
	//start PWR, BKP clock(from APB1)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);				//unlock BKP
	BKP_DeInit();								//BKP default init
	
	RCC_LSEConfig(RCC_LSE_ON);					//open erternal 32768Hz clock
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//wait for clock stable
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//32768Hz used as RTC source
	RCC_RTCCLKCmd(ENABLE);						//open RTC
	RTC_WaitForSynchro();						//wait for APB1 synchronize with RTC
	
	RTC_WaitForLastTask();						//confirm last operation is over
	RTC_SetPrescaler(32767);					//RTC period = RTCCLK/RTC_PR = (32768Hz)/(32767+1)
	RTC_WaitForLastTask();						//wait for operation is over
	RTC_ITConfig(RTC_IT_SEC, ENABLE);			//enable second interrupt    
	RTC_WaitForLastTask();						//wait for operation is over
	
	return;
}

/*******************************************************************************
* Function Name  : RTC_UserInit
* Description    : It is called when power on. Detect if it needs reconfigure.
* Input          : None
* Return         : None
*******************************************************************************/
void RTC_UserInit(void)
{
	//In BKP DR1, we stored a special character 0xA5A5. When forst power on, or
	//lost battery power, this charater will be lost. RTC needs reconfiguration.
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		RTC_ReConfig();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);//restore special character
	}
	else
	{
		//RTC is still running, doesn't need reconfiguration.
		//We can get reset type by RCC_GetFlagStatus()
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{//this is power on reset
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{//this is external RST pin reset
		}
		
		RCC_ClearFlag();
		//repeat some operations to ensure RTC is okay.
		RCC_RTCCLKCmd(ENABLE);					//open RTC
		RTC_WaitForSynchro();					//wait for APB1 synchronize with RTC
		
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//enable second interrupt  
		RTC_WaitForLastTask();					//wait for operation is over
	}
	RCC_ClearFlag();
	
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);/* Allow access to BKP Domain */
	// Disable the Tamper Pin To output RTCCLK/64 on Tamper pin
	BKP_TamperPinCmd(DISABLE); 
	/* Enable RTC Clock Output on Tamper Pin */
	
	return;
}

/*******************************************************************************
* Function Name  : GetTime
* Description    : Get time to struct *pCalendar from RTC.
* Input          : pointer to a calendar struct
* Return         : 
*******************************************************************************/
void GetTime(RTC_Def *pTimeNow)
{
	u32 t_t;
	if(SetTimeFlag)    return;					//lock RTC for set time
	
	t_t = RTC_GetCounter();						//call function in stm32f10x lib
	*pTimeNow = MyLocalTime(t_t, 0);			//call function in local file
}

/*******************************************************************************
* Function Name  : SetTime
* Description    : set time to RTC as demand in menu.
* Input          : pointer to a calendar struct 
* Return         : 
*******************************************************************************/
void SetTime(RTC_Def *pTimeNow)
{
	u32 t_t; 
	SetTimeFlag = 0;							//set time to RTC as demand in menu
	
	t_t = MyMakeTime(pTimeNow);					//call function in local file
	
	SoftDelayTimer = 20;						//timing out 20ms
	while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
	{											//Loop until RTOFF flag is set 
		if(SoftDelayTimer == 0) break;			//timing out
	}
	
	//  RTC_WaitForLastTask();
	RTC_SetCounter(t_t);						//call function in stm32f10x lib
	//  RTC_WaitForLastTask();
	
	SoftDelayTimer = 20;						//timing out 20ms
	while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
	{//Loop until RTOFF flag is set
		if(SoftDelayTimer == 0) break;			//timing out
	}
}

/*******************************************************************************
* Function Name  : GetDiffTime
* Description    : Form a 32 bit second counting value from calendar.
* Input          : pointers to 2 calendar structs
* Return         : 32 bit value in second
*******************************************************************************/
//u32 GetDiffTime(RTC_Def *pCalendar1, RTC_Def *pCalendar0)
//{
//	u32 t1, t0, dt;
//	
//	t1 = MyMakeTime(pCalendar1);				//call function in local file
//	t0 = MyMakeTime(pCalendar0);				//call function in local file
//	if(t1 > t0) dt = t1 - t0;					//get abs value
//	else dt = t0 - t1;							//get abs value
//	
//	return dt;
//}

/*******************************************************************************
* Function Name  : MyMakeTime
* Description    : Form a 32 bit second counting value from calendar.
* Input          : pointer to a calendar struct
* Return         : 32 bit second counting value
*******************************************************************************/
static u32 MyMakeTime(RTC_Def *pCalendar)
{
	u32 TotalSeconds = pCalendar->sec;
	s16 nYear = pCalendar->year;
	s16 nMonth = pCalendar->month;
	
	if((nYear < START_YEAR) || (nYear > (START_YEAR + 135))) 
		return 0;								//out of year range
	
	TotalSeconds += (u32)pCalendar->min * 60;	//contribution of minutes
	TotalSeconds += (u32)pCalendar->hour * 3600;//contribution of hours
	//contribution of mdays
	TotalSeconds += (u32)(pCalendar->mday - 1) * SEC_IN_DAY;
	
	if(IsLeap(nYear))
	{
		while(nMonth > 1)
		{
			TotalSeconds += (u32)mDayLeap[--nMonth] * SEC_IN_DAY;
		}
	}
	else
	{
		while(nMonth > 1)						//contribution of months
		{
	  		TotalSeconds += (u32)mDayNoLeap[--nMonth] * SEC_IN_DAY;
		}
	}
	
	while(nYear > START_YEAR)					//contribution of years
	{
		TotalSeconds += (u32)DAY_IN_YEAR(--nYear) * SEC_IN_DAY;
	}
	
	return TotalSeconds;
} 
  
/*******************************************************************************
* Function Name  : MyLocalTime
* Description    : Form a calendar from 32 bit second counting.
* Input          : pointer to a 32 bit second value, option pointer to a struct
* Return         : Calendar structure
*******************************************************************************/
static RTC_Def MyLocalTime(u32 TotalSecs, RTC_Def *pCalendar)
{
	RTC_Def Calendar;						//Local variables 
	u32 TotalDays, Remainder;
	  
	Calendar.year = START_YEAR;					//Calendar initialization
	Calendar.month = 1;  
	Calendar.mday = 1;
	Calendar.yday = 1;  
	Calendar.wday = Ymd2Wday(START_YEAR, 1, 1);
	
	TotalDays = TotalSecs/SEC_IN_DAY;			//Split days from seconds
	Remainder = TotalSecs%SEC_IN_DAY;
	
	Calendar.wday = (TotalDays + Calendar.wday) % 7;//Get week day
	
	Calendar.hour = Remainder/3600;				//Get clock in day
	Calendar.min = (Remainder/60)%60;
	Calendar.sec = Remainder%60;
	
	while(TotalDays >= DAY_IN_YEAR(Calendar.year))
	{
		TotalDays -= DAY_IN_YEAR(Calendar.year++);
	}
	Calendar.yday += TotalDays;					//Get years and days in year    
	
	if(IsLeap(Calendar.year))					//Get months and days in month
	{
		while(TotalDays >= mDayLeap[Calendar.month])
		{
	    	TotalDays -= mDayLeap[Calendar.month++];
		}
	}
	else
	{
		while(TotalDays >= mDayNoLeap[Calendar.month])
		{
	    	TotalDays -= mDayNoLeap[Calendar.month++];
		}
	}
	Calendar.mday += TotalDays;
	
	if(pCalendar)								//Copy Calendar if necessary
		*pCalendar = Calendar;
	
	return Calendar;
}

/*******************************************************************************
* Function Name  : Ymd2Wday
* Description    : Calculate days in week from year, month, mday.
* Input          : year, month, mday
* Return         : 0--6, Suanday--Saturday
*******************************************************************************/
static s16 Ymd2Wday(s16 nYear, s16 nMonth, s16 nMday)
{ 
	u8 i;
	if(IsLeap(nYear))
	{
		for(i = 1; i < nMonth; i++) nMday += mDayLeap[i]; 
	}
	else
	{
		for(i = 1; i < nMonth; i++) nMday += mDayNoLeap[i]; 
	}
	nYear--;
	return (nYear + nYear/4 - nYear/100 + nYear/400 + nMday)%7; 
}  

/***********************************************************************
* Function Name  : IsLeap
* Description    : Check whether the past year is leap or not.
* Input          : 4 digits year number
* Return         : 1: leap year. 0: not leap year
***********************************************************************/
static  u8  IsLeap(s16 nYear)
{
	if(nYear % 4 != 0)      return 0;
	if(nYear % 100 != 0)    return 1;
	return (u8)(nYear % 400 == 0);
}
/******************* (C) COPYRIGHT 2011 HCT *****END OF FILE****/
