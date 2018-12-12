/* calendar.h standard header */
#ifndef     _CALENDAR
#define     _CALENDAR

#include    <stm32f10x.h>


/***********************************************************************
*START_YEAR and START_WDAY are programmer determined definitions.
*Through those, a calendar from START_YEAR to START_YEAR+135 can be gotten.
*Do not need to care about local time or daylight saving time.
*When the user calibrates the calendar to a certain time zone, 
*it will run based on this zone.
***********************************************************************/
#define   START_YEAR      (2000)//default 2000, Jan.1, 00:00:00
#define   SEC_IN_DAY      (24*60*60)//one day includes 86400 seconds
#define   DAY_IN_YEAR(nYear) (IsLeap(nYear) ? 366 : 365)


typedef     struct
{       /* date and time components */
    s16     sec;    //senconds after the minute, 0 to 59
    s16     min;    //minutes after the hour, 0 to 59
    s16     hour;   //hours since midnight, 0 to 23
    s16     mday;   //day of the month, 1 to 31
    s16     month;  //months of the year, 1 to 12
    s16     year;   //years, START_YEAR to START_YEAR+135
    s16     wday;   //days since Sunday, 0 to 6
    s16     yday;   //days of the year, 1 to 366
}RTC_Def;

extern void RTC_UserInit(void);
void RTC_TimeGet(RTC_Def *Times);
static void RTC_ReConfig(void);
void GetTime(RTC_Def *pTimeNow);
void SetTime(RTC_Def *pTimeNow);
static u32 MyMakeTime(RTC_Def *pCalendar);
static RTC_Def MyLocalTime(u32 TotalSecs, RTC_Def *pCalendar);
static s16 Ymd2Wday(s16 nYear, s16 nMonth, s16 nMday);
static  u8  IsLeap(s16 nYear);


extern RTC_Def TimeNow;

#endif/* _CALENDAR */
/***************************END OF FILE********************************/
