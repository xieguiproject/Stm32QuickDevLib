#ifndef _GPS_H_
#define _GPS_H_
#include "main.h"

typedef struct tagNmea_slmsg
{
	uint8_t num;			/*卫星编号*/
	uint8_t eledeg;		/*卫星仰角*/
	uint16_t azideg;	/*卫星方位角*/
	uint8_t sn;				/*信噪比*/
}Nmea_slmsg;

typedef struct tagNmea_utc_time
{
	uint16_t year;	
	uint8_t month;	
	uint8_t date;	
	uint8_t hour; 	
	uint8_t min; 	
	uint8_t sec; 	
}Nmea_utc_time;

/*通信相关定义，主要描述串口相关的东西*/
typedef struct tagGpsComInfo_s
{
	uint8_t  RecvMessage[1024];		/*接受缓存*/
	//	uint8_t  SendMessage[GSM_SEND_BUF_LEN];										/*发送缓存*/
	uint16_t RecvCount;															/*当前接受个数*/
	uint16_t SendCount;															/*本次发送数据长度*/
	uint32_t  WorkFlage;	/*串口工作状态标志*/														/*状态*/
	
}GpsComInfo;


/*快速命令检测需要的变量*/
typedef struct tagGPSFastCmd_s
{
	uint8_t   RecvBuf[512];
	uint8_t   RecvCount;									/*快速命令存储空间*/
	uint8_t   RecvFlage;									/*快速命令标志，1标志检测到命令头部0，表示未检测到*/
	uint8_t   CheckHead;									/*检测到完整头部*/
}GPSFastCmdInfo;

typedef struct tagGPS_s
{
	uint8_t svnum;/*可见卫星个数*/
	Nmea_slmsg slMsg[12];	/*卫星信息*/
	Nmea_utc_time utc;/*时间信息*/
	double latitude;				/**纬度*/
	
	uint8_t nshemi;					//	北纬/南纬N或者S		  
	double longitude;			    /*经度*/
	uint8_t ewhemi;					//E东经W西经
	uint8_t gpssta;					/*gps状态0未定位1非差分定位2差分定位6正在估算*/
 	uint8_t posslnum;				//卫星个数0~12.
 	uint8_t possl[12];				//定位卫星编号
	uint8_t fixmode;					//定位类型1没有定位2 2D定位 3D定位
	uint16_t pdop;					//位置精度因子0~500,实际0~50.0
	uint16_t hdop;					//水平精度因子0~500,实际0~50.0
	uint16_t vdop;					//垂直精度因子0~500,实际0~50.0 

	int altitude;			 	  //海拔高度 
	uint16_t speed;					//地面速率
	USARTInfo_s *Usart;				/*模块绑定的串口*/
	GPSFastCmdInfo GpsGpgsa;       /*gpgsa信息*/
}GPS;

extern int8_t  AnalysisGpgsa(uint8_t *buf,uint16_t len);
extern uint8_t GpsInit(void);
extern void GpsFastGPGSACheck(uint8_t arg);
extern uint8_t PaketGpsState(char *cGpsStatBuf,uint16_t bufSize);

extern GPS mdGps;

#endif