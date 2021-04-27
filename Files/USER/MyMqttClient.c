
#include "..\USER\MyMqttClient.h"
#include "..\MQTT\MQTTPacket.h" 
#include "..\MQTT\Transport.h"
#include "usart3.h"
#include "..\SYSTEM\SIM868.h"


const int UART_SIM800=3;
const u16 CMD_DELAY=1000;

uint8_t mqtt_receive = 0;
char mqtt_buffer[1460] = {0};
uint16_t mqtt_index = 0;


SIM800_t SIM800;

void InitSim800(void)
{
    //if(SIM800==NULL) 
	
    SIM800.sim.apn = "CMNET";
    SIM800.sim.apn_user = "";
    SIM800.sim.apn_pass = "";
    SIM800.mqttServer.host = "mqtt-2.omnivoltaic.com";
    SIM800.mqttServer.port = 1883;
    SIM800.mqttClient.username = "Wisdon";
    SIM800.mqttClient.pass = "123456";
    SIM800.mqttClient.clientID = "TestSub";
    SIM800.mqttClient.keepAliveInterval = 120;	
	
};
//===========================================================================
/*


*/
int  HAL_UART_Transmit_IT(int huart, uint8_t *pData, uint16_t Size)
{
	int re=Uart3.SendData(pData,Size,1000);
	return re;
}

int HAL_UART_Receive_IT(int huart, uint8_t *pData, uint16_t Size)
{
	  int re=Uart3.Receive( pData,Size,1000);
		return re;
}
void clearRxBuffer(void) {
    //rx_index = 0;
   // memset(rx_buffer, 0, sizeof(rx_buffer));
	 Uart3.ClearRxBuff();
}
void HAL_Delay(int delay)
{
	 delay_ms(1000);   
	 delay_ms(500);   
}
//===========================================================================
/**
 * Clear MQTT buffer.
 * @param NONE
 * @return NONE
 */
void clearMqttBuffer(void) {
    mqtt_receive = 0;
    mqtt_index = 0;
    memset(mqtt_buffer, 0, sizeof(mqtt_buffer));
}
//===========================================================================
/**
 * Send AT command to SIM800 over UART.
 * @param command the command to be used the send AT command
 * @param reply to be used to set the correct answer to the command
 * @param delay to be used to the set pause to the reply
 * @return error, 0 is OK
 */
int SIM800_SendCommand(char *command, char *reply, uint16_t delay) {

	  HAL_UART_Transmit_IT(UART_SIM800, (unsigned char *) command,  (uint16_t) strlen(command));

#if FREERTOS == 1
    osDelay(delay);
#else
    HAL_Delay(delay);
	 
	  if(delay>=3000)
		{
			HAL_Delay(1000);HAL_Delay(1000);HAL_Delay(1000);
			HAL_Delay(1000);HAL_Delay(1000);HAL_Delay(1000);				
		}
#endif 
		memcpy(mqtt_buffer, Uart3._Rxbuf,Uart3._RxNum  );
		
    if (strstr(mqtt_buffer, reply) != NULL) {
        clearRxBuffer();
			  clearMqttBuffer();
			  //for debug
			  USART_DEBUG_SendStr(command);  USART_DEBUG_SendStr("=OK\r\n");	
			
        return 0;
    }
		
		USART_DEBUG_SendStr(command);  USART_DEBUG_SendStr("=OK\r\n");		
    clearRxBuffer();
		clearMqttBuffer();
    return 1;
}

//===========================================================================
/**
 * initialization SIM800.
 * @param NONE
 * @return error status, 0 - OK
 */
int MQTT_Init(void) {
    SIM800.mqttServer.connect = 0;
    int error = 0;
//    char str[32] = {0};
//  HAL_UART_Receive_IT(UART_SIM800, &rx_data, 1);
		
		Omnivoltaic_GPRSConnctTcp("1",1883);
		
		SIM800.mqttServer.connect=1;
		
    if (error == 0) {
        MQTT_Connect();
        return error;
    } else {
        return error;
    }
}

//===========================================================================
/**
 * Connect to MQTT server in Internet over TCP.
 * @param NONE
 * @return NONE
 */
u8 rx_data[100]={0};

void MQTT_Connect(void) {
	
	  int serverok=0;
    SIM800.mqttReceive.newEvent = 0;
  //  SIM800.mqttServer.connect = 0;
    char str[128] = {0};
    unsigned char buf[128] = {0};

#if FREERTOS == 1
    osDelay(5000);
#else
    HAL_Delay(5000);
#endif
    if (SIM800.mqttServer.connect == 1) {
        MQTTPacket_connectData datas = MQTTPacket_connectData_initializer;
        datas.username.cstring = SIM800.mqttClient.username;
        datas.password.cstring = SIM800.mqttClient.pass;
        datas.clientID.cstring = SIM800.mqttClient.clientID;
        datas.keepAliveInterval = SIM800.mqttClient.keepAliveInterval;
        datas.cleansession = 1;
        int mqtt_len = MQTTSerialize_connect(buf, sizeof(buf), &datas);
        HAL_UART_Transmit_IT(UART_SIM800, buf, mqtt_len);
			
			  HAL_UART_Receive_IT(UART_SIM800, rx_data, 10);//for debug
			
#if FREERTOS == 1
        osDelay(5000);
#else
        HAL_Delay(5000);
#endif
    }
}

//===========================================================================
/**
 * Public on the MQTT broker of the message in a topic
 * @param topic to be used to the set topic
 * @param payload to be used to the set message for topic
 * @return NONE
 */
void MQTT_Pub(char *topic, char *payload) {
    unsigned char buf[256] = {0};

    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = topic;

    int mqtt_len = MQTTSerialize_publish(buf, sizeof(buf), 0, 0, 0, 0,
                                         topicString, (unsigned char *) payload, (int) strlen(payload));
		
		Uart1_SendData2(buf,mqtt_len,1);//for debug	
		
    HAL_UART_Transmit_IT(UART_SIM800, buf, mqtt_len);
}

//===========================================================================
/**
 * Send a PINGREQ to the MQTT broker (active session)
 * @param NONE
 * @return NONE
 */
void MQTT_PingReq(void) {
    unsigned char buf[16] = {0};

    int mqtt_len = MQTTSerialize_pingreq(buf, sizeof(buf));
    HAL_UART_Transmit_IT(UART_SIM800, buf, mqtt_len);
}

//===========================================================================
/**
 * Subscribe on the MQTT broker of the message in a topic
 * @param topic to be used to the set topic
 * @return NONE
 */
void MQTT_Sub(char *topic) {
    unsigned char buf[256] = {0};

    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = topic;

    int mqtt_len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, 1, 1,
                                           &topicString, 0);
		
    HAL_UART_Transmit_IT(UART_SIM800, buf, mqtt_len);
#if FREERTOS == 1
    osDelay(5000);
#else
    HAL_Delay(5000);
#endif
}

//===========================================================================
/**
 * Receive message from MQTT broker
 * @param receive mqtt bufer
 * @return NONE
 */
void MQTT_Receive(unsigned char *buf) {
    memset(SIM800.mqttReceive.topic, 0, sizeof(SIM800.mqttReceive.topic));
	
    memset(SIM800.mqttReceive.payload, 0, sizeof(SIM800.mqttReceive.payload));
	
    MQTTString receivedTopic;
	
    unsigned char *payload;
	
    MQTTDeserialize_publish(&SIM800.mqttReceive.dup, &SIM800.mqttReceive.qos, &SIM800.mqttReceive.retained,
                            &SIM800.mqttReceive.msgId,
                            &receivedTopic, &payload, &SIM800.mqttReceive.payloadLen, buf,
                            sizeof(buf));
    memcpy(SIM800.mqttReceive.topic, receivedTopic.lenstring.data, receivedTopic.lenstring.len);
    SIM800.mqttReceive.topicLen = receivedTopic.lenstring.len;
    memcpy(SIM800.mqttReceive.payload, payload, SIM800.mqttReceive.payloadLen);
    SIM800.mqttReceive.newEvent = 1;
}


#if 0

/**
 * Call back function for release read SIM800 UART buffer.
 * @param NONE
 * @return NONE
 */

void Sim800_RxCallBack(void) {
    rx_buffer[rx_index++] = rx_data;

    if (SIM800.mqttServer.connect == 0) {
        if (strstr((char *) rx_buffer, "\r\n") != NULL && rx_index == 2) {
            rx_index = 0;
        } else if (strstr((char *) rx_buffer, "\r\n") != NULL) {
            memcpy(mqtt_buffer, rx_buffer, sizeof(rx_buffer));
            clearRxBuffer();
            if (strstr(mqtt_buffer, "DY CONNECT\r\n")) {
                SIM800.mqttServer.connect = 0;
            } else if (strstr(mqtt_buffer, "CONNECT\r\n")) {
                SIM800.mqttServer.connect = 1;
            }
        }
    }
    if (strstr((char *) rx_buffer, "CLOSED\r\n") || strstr((char *) rx_buffer, "ERROR\r\n")) {
        SIM800.mqttServer.connect = 0;
    }
    if (SIM800.mqttServer.connect == 1 && rx_data == 48) {
        mqtt_receive = 1;
    }
    if (mqtt_receive == 1) {
        mqtt_buffer[mqtt_index++] = rx_data;
        if (mqtt_index > 1 && mqtt_index - 1 > mqtt_buffer[1]) {
            MQTT_Receive((unsigned char *) mqtt_buffer);
            clearRxBuffer();
            clearMqttBuffer();
        }
        if (mqtt_index >= sizeof(mqtt_buffer)) {
            clearMqttBuffer();
        }
    }
    if (rx_index >= sizeof(mqtt_buffer)) {
        clearRxBuffer();
        clearMqttBuffer();
    }
    HAL_UART_Receive_IT(UART_SIM800, &rx_data, 1);
}
//*/

#endif


