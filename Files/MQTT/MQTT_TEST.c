#include "MQTT_TEST.H"
#include "string.h"

#include "usart3.h" 

#define KEEPALIVE_INTERVAL 1000

int toStop = 0;

unsigned char buf[200];
int buflen = sizeof(buf);

MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
MQTTString topicString=MQTTString_initializer;


void MQTT_DEBUG(char* debugstr,int delay);
void MQTT_Delay(uint16_t delay);
int  MQTT_Send_Packet_PING(u16 delay);
int  MQTT_Send_Packet_CONNECT(uint16_t delay);


/****************************************************************************************************
*MQTT 包头初始化 函数
*
*
****************************************************************************************************/	
int MQTT_Send_Packet_Init(char* clientID, char* username, char*password,char * topicstr, u8 MQTTVersion, u16 keepAliveInterval,u8 cleansession  )
{
	
	data.clientID.cstring  = clientID;//"OVS_";
	data.keepAliveInterval =keepAliveInterval; //KEEPALIVE_INTERVAL;
	data.cleansession      =cleansession; //1;
	data.username.cstring  =username; //"adminWISDON";
	data.password.cstring  =password; //"admin123";
  data.MQTTVersion       =MQTTVersion; //4;	
	
	topicString.cstring    =topicstr;
	
	return 1;
}

/****************************************************************************************************
*MQTT CONNECT 函数
*
*
****************************************************************************************************/
int MQTT_Send_Packet_CONNECT(uint16_t delay)
{
	int mysock=0;
	int len=0;
	int rc=0;	
		
	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);	
	
	printf("Sent MQTT connect\n");
	/* wait for connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
		    //printf("Unable to connect, return code %d\n", connack_rc);	
			  MQTT_DEBUG("Unable to build MQTTConnect",1000);//debug
			
			  goto exit;
		}
		else
		{
			  MQTT_DEBUG("MQTTConnected OK!\r\n\r\n",1000);//debug	
			  return 1;
		}			
		
	}	

	
exit:
	transport_close(mysock);
	
	return 0;	
}

/****************************************************************************************************
*MQTT PUBLISH 函数
*topicstr 
*payload
*payloadlen
****************************************************************************************************/
int  MQTT_Send_Packet_PUBLISH(char* topicstr,  u8 * payload,  int payloadlen)
{
	int mysock=0;	
	int len=0;
	int rc=0;	

	//"_OVES/ABCD/1234"
	
	memset(buf,0,buflen);//wis
	len = MQTTSerialize_connect((unsigned char *)buf, buflen, &data);

	topicString.cstring = topicstr;//"mytopic_wisdon";
	len += MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

	len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);

	rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
	if (rc == len){
		
		//printf("Successfully published\n");
		MQTT_DEBUG("\r\n_wisdon Successfully published!\r\n",1000);		
		
	}
	else{
		//printf("Publish failed\n");	
		MQTT_DEBUG("\r\n_wisdon Publish failed!\r\n",1000);
	
	}
 		  while(1)//for debug
			{
			  MQTT_Delay(1000);
			  MQTT_Delay(1000);				
			}		
	
exit:
	transport_close(mysock);
	
	
	return 0;	
}


/****************************************************************************************************
*MQTT SUBSCRIBE 函数
*subtopicstr 
*
*
****************************************************************************************************/
int  MQTT_Send_Packet_SUBSCRIBE(char* subtopicstr)
{
	int mysock=0;	
	int len=0;
	int rc=0;	
	int msgid = 1;
  int req_qos = 0;
//  unsigned char * buf=0;
//	int buflen=0;		
	

	/* subscribe */
	topicString.cstring = subtopicstr;//"substopic";
	
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	rc = transport_sendPacketBuffer(mysock, buf, len);
	
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			goto exit;
		}
	}
	else
		goto exit;
	
	
	
exit:
	transport_close(mysock);
	
	
	return 0;	
}



/****************************************************************************************************
*MQTT Read_Packet_SUBSCRIBE 函数
*topicstr 
*
*
****************************************************************************************************/
int  MQTT_Read_Packet_SUBSCRIBE(char* subtopicstr)
{
	int mysock=0;	
	int len=0;
	int rc=0;	
	int msgid = 1;
	
	/* loop getting msgs on subscribed topic */
	topicString.cstring = "pubtopic";
	while (!toStop)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
//			int rc=0;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
		}

		//printf("publishing reading\n");
		//len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
		//rc = transport_sendPacketBuffer(mysock, buf, len);
	}


   return 0;
}

/****************************************************************************************************
*MQTT TEST 函数
*
*
****************************************************************************************************/
int  MQTT_Send_Packet_Test()
{

	int mysock=0;
	int msgid = 1;
  int req_qos = 0;

	int len=0;
	int rc=0;

	MQTT_Send_Packet_Init( "OVS_","adminWISDON","admin123", "mytopic_wisdon",4,1000,1);
	//===============================================================	
	//connect
	MQTT_Send_Packet_CONNECT(1);
	
			while(0)
			{			
				MQTT_Delay(1000);
			  MQTT_Delay(1000);				  
			//	MQTT_Delay(1000);
			//  MQTT_Delay(1000);			
			  
        MQTT_Send_Packet_PING(1000);							
			}
	 
//  MQTT_Send_Packet_PING(1000);	
	//===============================================================	
	//publish
	/*		
		//	char* publishtopic="_OVES//test_topic_wisdon";
			memset(buf,0,buflen);//wis
			char* publishtopic="test_topic_wisdon";
			char* payloadstr  =":{__payload of wisdon}";
			int   payloadlen=strlen(payloadstr);
			
				MQTT_Send_Packet_PUBLISH(publishtopic,(u8*)payloadstr,payloadlen);	
			
		while(1)
		{			
			
	    //	MQTT_Send_Packet_PUBLISH(publishtopic,(u8*)payloadstr,payloadlen);	
			
			  MQTT_Delay(1000);
		//	  MQTT_Delay(1000);			
		//	  MQTT_Delay(1000);			
		}
	*/
	//===============================================================	
	//publish
	//*
	
	char* payload = "mypayload";
	int payloadlen = strlen(payload);
	
	MQTT_Send_Packet_PING(1000);
	
	memset(buf,0,buflen);//wis
	
	len = MQTTSerialize_connect((unsigned char *)buf, buflen, &data);

	topicString.cstring = "mytopic_wisdon123";
	len += MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

	len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);

	rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
	if (rc == len){
		
		//printf("Successfully published\n");
		char* tip ="wisdon Successfully published\r\n";		
		Uart1_SendData2((u8*)tip,strlen(tip),1000);	
	}
	else{
		//printf("Publish failed\n");	
		char* tip ="Publish failed from wisdon\r\n";
			
    Uart1_SendData2((u8*)tip,strlen(tip),1000);	
		
	}
	
 		  while(1)
			{
			  MQTT_Delay(1000);
			  MQTT_Delay(1000);		
			  MQTT_Delay(1000);	
				
	      MQTT_Send_Packet_PING(1000);				
				
			}	
	//*/
	//===============================================================
		/* subscribe */

	topicString.cstring = "substopic";
	
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	rc = transport_sendPacketBuffer(mysock, buf, len);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) 	/* wait for suback */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			goto exit;
		}
	}
	else
		goto exit;
	

	//=================================================================
	/* loop getting msgs on subscribed topic */
	
	
	topicString.cstring = "pubtopic";
	while (!toStop)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char* payload_in;
//			int rc=0;
			MQTTString receivedTopic;

			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %.*s\n", payloadlen_in, payload_in);
		}

//		printf("publishing reading\n");
//		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payloadstr, payloadlen);
//		rc = transport_sendPacketBuffer(mysock, buf, len);
	}

	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);	
	
	

exit:
	transport_close(mysock);
	
	
	return 0;
}



/****************************************************************************************************
*MQTT 心跳发送专用函数
*
****************************************************************************************************/

int MQTT_Send_Packet_PING(u16 delay)
{
		int mysock=0;
	//	unsigned char buf[4]={0};
	//	int buflen=4;	
		int len=0;

    MQTT_Delay(delay);
		
		len = MQTTSerialize_pingreq(buf, buflen);

		transport_sendPacketBuffer(mysock, buf, len);
		//	printf("Ping...");

	
	  MQTT_Delay(1000);
	
	 //return 1;
	  
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PINGRESP){
				
			   MQTT_DEBUG("PingOK\r\n",1000);//debug			
			   //printf("Pong\n");
				 //start_ping_timer();
			   return 1;
		}
		else {			
				MQTT_DEBUG("OOPS!!\r\n",1000);//debug
				//printf("OOPS\r\n");
				goto exit;
		}		

exit:
	transport_close(mysock);
	
	
	return 0;
} 
/****************************************************************************************************
*MQTT 专用毫秒延时函数，不用其他函数
*
****************************************************************************************************/
void MQTT_Delay(uint16_t delay)
{
			delay_ms(delay);
}

/****************************************************************************************************
*MQTT 专用DEBUG函数，不用其他函数
*
****************************************************************************************************/
void MQTT_DEBUG(char* debugstr,int delay)
{
		//	delay_ms(delay);
	    Uart1_SendData2((u8*)debugstr,strlen(debugstr),1000);	
	
}
