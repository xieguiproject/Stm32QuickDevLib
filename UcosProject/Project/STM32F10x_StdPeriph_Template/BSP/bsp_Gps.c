#include "Gps.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
GPS mdGps;
/*解析GPGSA信息*/

int8_t  AnalysisGpgsa(uint8_t *buf,uint16_t len)
{
	uint8_t temp = 0;
	uint32_t temp32 = 0;
	/*gps时间和真实时间差8个小时*/
	temp = (8 + ((buf[7] - 0x30) * 10) +  (buf[8] - 0x30));
	if(temp >= 0 && temp <= 24)
	{
		mdGps.utc.hour = temp;
	}
	
	temp = (((buf[9] - 0x30) * 10) + (buf[10] - 0x30));
	if(temp >=0 && temp < 24)
	{
		mdGps.utc.min = temp;
	}
	
	temp = (((buf[11] - 0x30) * 10) + (buf[12] - 0x30));
	if(temp >=0 && temp < 24)
	{
		mdGps.utc.sec = temp;
	}
	
	buf[27] = 0;/*掐断字符*/
	temp32 = (uint32_t)(atof((char *)&buf[17])* 10000);
	if(temp32 >= 0 && temp32 <= 4294967295)
	{
		mdGps.latitude = temp32;/*纬度信息*/
	}
	if(buf[28] == 'N' || buf[28] == 'S')
	{
		mdGps.nshemi = buf[28];/*南北维*/
	}

	buf[41] = 0;/*掐断字符*/
	temp32 = (uint32_t)(atof((char *)&buf[30])* 10000);
	if(temp32 >= 0 && temp32 <= 4294967295)
	{
		mdGps.longitude = temp32;/*纬度信息*/
	}
	if(buf[42] == 'E' || buf[42] == 'W')
	{
		mdGps.ewhemi = buf[42];/*南北维*/
	}
	/*定位类型*/
	mdGps.fixmode = buf[44] - 0x30;
	/*其余的暂时不解析*/
	return 1;
	
}
/*用于快速检测GPGSAC信息*/
void GpsFastGPGSACheck(uint8_t arg)
{
	if(arg == '$')/**/
	{
		mdGps.GpsGpgsa.RecvCount = 0;
		mdGps.GpsGpgsa.CheckHead = 0; 
		mdGps.GpsGpgsa.RecvFlage = 1;/*暂时代表检测到头部信息*/
		mdGps.GpsGpgsa.RecvBuf[mdGps.GpsGpgsa.RecvCount] = arg;
	  return;
	}
	else if(mdGps.GpsGpgsa.RecvFlage == 1 && mdGps.GpsGpgsa.CheckHead == 0)
	{
		mdGps.GpsGpgsa.RecvCount++;
		mdGps.GpsGpgsa.RecvBuf[mdGps.GpsGpgsa.RecvCount] = arg;
		/*头部信息*/
		if(mdGps.GpsGpgsa.RecvCount >= 7)
		{
			/*开始检测是否是一个完整的GPGSA信息*/
			if(NULL != strstr(mdGps.GpsGpgsa.RecvBuf,"$GPGGA"))
			{
				mdGps.GpsGpgsa.CheckHead = 1;/*检测到完整的头部信息*/
				return;
			}
			else
			{
				memset((char *)&mdGps.GpsGpgsa,0,sizeof(GPSFastCmdInfo));
				return;
			}
		}
	}
	if(mdGps.GpsGpgsa.CheckHead == 1)
	{
		mdGps.GpsGpgsa.RecvCount++;
		mdGps.GpsGpgsa.RecvBuf[mdGps.GpsGpgsa.RecvCount] = arg;
		/*开始检测尾部*/
		if(arg == '\n' && mdGps.GpsGpgsa.RecvBuf[mdGps.GpsGpgsa.RecvCount - 1] == '\r')
		{
			AnalysisGpgsa(mdGps.GpsGpgsa.RecvBuf,mdGps.GpsGpgsa.RecvCount);
			/*检测到一包完整的GPGSA数据，下面可进行解析*/
			//printf("解析完成\n");
			memset((char *)&mdGps.GpsGpgsa,0,sizeof(GPSFastCmdInfo));
		}
	}
}
/*打包GPS状态*/
uint8_t PaketGpsState(char *cGpsStatBuf,uint16_t bufSize)
{
	/*这里对GPS信息进行打包处理*/
	memset(cGpsStatBuf,0,bufSize);
	/*打包格式经度维度*/
	sprintf(cGpsStatBuf,"%c%d-%c%d",mdGps.ewhemi,mdGps.longitude,mdGps.nshemi,mdGps.latitude);
	
	
}

uint16_t GpsRecvCmdDel(uint16_t arg1,uint16_t arg2)
{
	mdGps.gpsComInfos.WorkFlage |= RECV_IS_ING;	/*正在接受*/
	
	mdGps.gpsComInfos.RecvMessage[mdGps.gpsComInfos.RecvCount] = arg1;
	GpsFastGPGSACheck(arg1);/*检测快速命令*/
	mdGps.gpsComInfos.RecvCount++;
	
	if(mdGps.gpsComInfos.RecvCount > 2)
	{
		mdGps.gpsComInfos.WorkFlage |= 0x01;/*接受到新数据*/
	}
	if(mdGps.gpsComInfos.RecvCount > 1023)
	{
		/*未处理*/
		mdGps.gpsComInfos.RecvCount = 0;
	}
	mdGps.gpsComInfos.WorkFlage &=~RECV_IS_ING;	/*正在接受*/
	return 1;
}

uint8_t GpsInit(void)
{
	memset(&mdGps,0,sizeof(GPS));
	
	USARTInit(USART2, 9600);
	mdGps.gpsComInfos
	mdUsart.USART3_s.USARTUserDel = GpsRecvCmdDel;
	/**/
	
}