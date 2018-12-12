#ifndef BSP_ONENETSERVER_H
#define BSP_ONENETSERVER_H

#include "stm32f10x.h"			
#include "os.h"				/*需要基于操作系统*/
#include "BspConfig.h"
#include "Bsp_Esp8266.h"

#define MQTT_SERVER_IP 		"183.230.40.39"
#define MQTT_SERVER_PORT  6002

/** MQTT数据包类型 */
enum MqttPacketType {
    MQTT_PKT_CONNECT = 1, /**< 连接请求数据包 */
    MQTT_PKT_CONNACK,     /**< 连接确认数据包 */
    MQTT_PKT_PUBLISH,     /**< 发布数据数据包 */
    MQTT_PKT_PUBACK,      /**< 发布确认数据包 */
    MQTT_PKT_PUBREC,      /**< 发布数据已接收数据包，Qos 2时，回复MQTT_PKT_PUBLISH */
    MQTT_PKT_PUBREL,      /**< 发布数据释放数据包， Qos 2时，回复MQTT_PKT_PUBREC */
    MQTT_PKT_PUBCOMP,     /**< 发布完成数据包， Qos 2时，回复MQTT_PKT_PUBREL */
    MQTT_PKT_SUBSCRIBE,   /**< 订阅数据包 */
    MQTT_PKT_SUBACK,      /**< 订阅确认数据包 */
    MQTT_PKT_UNSUBSCRIBE, /**< 取消订阅数据包 */
    MQTT_PKT_UNSUBACK,    /**< 取消订阅确认数据包 */
    MQTT_PKT_PINGREQ,     /**< ping 数据包 */
    MQTT_PKT_PINGRESP,    /**< ping 响应数据包 */
    MQTT_PKT_DISCONNECT   /**< 断开连接数据包 */
};


/*mqtt协议头部*/
typedef struct tagMQTTHeader_s
{
	uint8_t ucPacketType;					/*mqtt包数据类型*/
	
	uint8_t ucRemainingLenth;				/*剩余长度*/
}MQTTHeader;

/*mqtt连接协议*/
typedef struct tagMQTTConnect_s
{
	MQTTHeader sHeader;						/*固定头部*/
	uint16_t uiProtocolNameLength;		    /*协议名称长度*/
	uint8_t 	uiMqttName[4];				/*协议名称*/
	uint8_t ucProtocolLevel;				/*协议等级*/
	uint8_t ucConnectFlag;					/*状态标识*/
	uint16_t ucKeepAliveTime;				/*在线时间*/
											/*其他的信息手动填写*/
}MQTTConnect;

typedef struct tagMQTTHeart_s
{
	MQTTHeader sHeader;							/*固定头部*/
}MQTTHeart;

typedef struct tagMQTTPush_s
{
	MQTTHeader sHeader;							/*固定头部*/
	uint8_t ucRemainingLenth2;				/*剩余长度*/
}MQTTPush;


#define USER_BUFF_LEN									25	
#define CONNECT_USER_FLAGE 								0X80					/*用户flag*/
#define CONNECT_PASSWD_FLAGE							0X40


void OnenetConnectMqttServer(uint8_t *ucMqttServerIp,uint16_t uiMqttServerPort);
void OnenetMqttConnect(uint16_t KeepOnlineTime,char *DeviceId,
							 char *UserName,char *Password);
void OnenetKeepAliveInit();
uint16_t MQTTPublist(char *TopicName,float fCenterCm,float fHightCm,float fLowCm,
									uint16_t uiSysStatusCode,uint16_t uiWaterPercent);


#endif
