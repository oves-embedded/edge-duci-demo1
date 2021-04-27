#include "MQTT_TEST.H"
#include "string.h"

#include "usart3.h" 

#define KEEPALIVE_INTERVAL 1000

int toStop = 0;

unsigned char gbuf[200];
int gbuflen = sizeof(gbuf);

MQTTPacket_connectData gdata = MQTTPacket_connectData_initializer;
MQTTString gtopicString=MQTTString_initializer;


void MQTT_DEBUG(char* debugstr,int delay);
void MQTT_DEBUG2(char* buff,int bufflen, char type);
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
	//
	//	MQTT_Send_Packet_Init( "_OVES","WIS","123", "topic",4,1000,1);
	
	gdata.clientID.cstring  = clientID;//"_OVES";
	gdata.keepAliveInterval =keepAliveInterval; //KEEPALIVE_INTERVAL;
	gdata.cleansession      =cleansession; //1;
	gdata.username.cstring  =username; //"adminWISDON";
	gdata.password.cstring  =password; //"admin123";
  gdata.MQTTVersion       =MQTTVersion; //4;	
	
	gtopicString.cstring    =topicstr;
	
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
	unsigned char buf[200];
	int buflen = sizeof(buf);	

	len = MQTTSerialize_connect(buf, buflen, &gdata);
	
	//MQTT_DEBUG2((char*)buf ,len, 1);
	//MQTT_DEBUG2((char*)buf ,len, 0);
	
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

	unsigned char buf[200];
	int buflen = sizeof(buf);	
	//"_OVES/ABCD/1234"
	

  MQTTString topicString=MQTTString_initializer;	
	
	topicString.cstring = topicstr;//"mytopic_wisdon";
	len = MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);

	//len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);
		
	rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
	if (rc == len){
		printf("Successfully published\n");
		//MQTT_DEBUG("\r\n_wisdon Successfully published!\r\n",1000);			
	}
	else{
		printf("Publish failed\n");	
		//MQTT_DEBUG("\r\n_wisdon Publish failed!\r\n",1000);
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
	unsigned char buf[200];
	int buflen = sizeof(buf);
	

	MQTTString topicString=MQTTString_initializer;
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
	
	unsigned char buf[200];
	int buflen = sizeof(buf);	
	
	/* loop getting msgs on subscribed topic */
	MQTTString topicString=MQTTString_initializer;
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
void MQTT_DEBUG2(char* buff,int bufflen, char type   )
{
   int lent=0;
	 char buftt[30]={0};
	 
   MQTT_DEBUG("\r\n_send data start_\r\n",1000);
  
	 
	 lent=sprintf( buftt  , "\r\ntotal %d byte\r\n"   ,bufflen ); 
		Uart1_SendData2((u8*)buftt,lent,100);
	 

  for(int i=0;i<bufflen;i++)
  {	 
		 if(type==1)lent=sprintf( buftt  , "0x%02x," ,buff[i]  ); 
		 else       lent=sprintf( buftt  , "%02x,"   ,buff[i]  ); 
		 Uart1_SendData2((u8*)buftt,lent,100);
	}

   MQTT_DEBUG("\r\n_send data end_\r\n",1000);	

}


/****************************************************************************************************
*MQTT TEST 函数
*
*
****************************************************************************************************/
int  MQTT_Send_Packet_Test()
{
		
	int rc = 0;
	int mysock = 0;
	char* payload = "{att{  },sta{ },sts{x=1,y=2,z=3}}";
	int payloadlen = strlen(payload);

	MQTT_Send_Packet_Init( "me","wis","123", "mytopic",4,520,1);
	
	MQTT_DEBUG("\r\n------------connet-------------\r\n",1000);
	
	MQTT_Send_Packet_CONNECT(1000);




	while(1)
	{
		
		delay_ms(1000);

		
	  MQTT_DEBUG("\r\n------------public-------------\r\n",1000);

	  MQTT_Send_Packet_PUBLISH("_OVES/wistopic",(u8*)payload, payloadlen);		
	
		delay_ms(1000);
		delay_ms(1000);		
		delay_ms(1000);
		delay_ms(1000);		
		
		MQTT_Send_Packet_PING(1000);
		
	}
	

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
		unsigned char buf[4]={0};
		int buflen=4;	
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
