#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart3.h"
#include "..\SYSTEM\SIM868.h"


//#include "..\MQTTSim800\MQTTSim800.h"

//#include "..\USER\MyMqttClient.h"
//#include "..\MQTT\mqttclient.h"
#include "..\MQTT\MQTTPacket.h" 
#include "..\MQTT\Transport.h"
#include "..\MQTT\MQTT_TEST.H"


int MQTT_TASK2(void);

u8 sendbuf[200]={0};
u8 recvbuf[200]={0};

int main(void)
 {	
//	u8 t;
//	u8 len;	
	u16 times=0; 
 
	delay_init();	    	 //ÑÓÊ±º¯Êý³õÊ¼»¯	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶·Ö×é2
	uart_init(38400);	 //´®¿Ú³õÊ¼»¯Îª9600
	 
//	testloop() ;
	 
	Uart3.InitUart(38400);	 //´®¿Ú³õÊ¼»¯Îª9600

	 
	LED_Init();		  	 //³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú 
 
	//Omnivoltaic_GPRSConnctTcp("mqtt-2.omnivoltaic.com",1883);
	 
  //SendDataToSim800c((u8*)"test",4,1000);
	 
  MQTT_TASK2();
	 
	 
	// Omnivoltaic_GPRSConnctTcp("1",1883);
	 
//	  Omnivoltaic_GPRSCommand("ATE0\r\n", "OK", 40); 
	 
	
	while(1)
	{
				times++;			
			 // if(times%500==0)
				{
				//	len=sprintf((char*)sendbuf,"times=%06d\r\n\r\n",times );
				//	Uart3.SendData(sendbuf,len,1000);				
				//	len=sprintf((char*)sendbuf,"ATE0\r\n" );
				//	Uart3.SendData(sendbuf,len,1000);				

				}
					
				//	printf("%06d\r\n",times );	
			 // 	Uart3.Receive(recvbuf,1,1000);
			  // delay_ms(10);
			 // 	Uart3.SendData(recvbuf,1,1000);
				
			//	if(times%30==0)LED0=!LED0;//ÉÁË¸LED,ÌáÊ¾ÏµÍ³ÕýÔÚÔËÐÐ.
			//	delay_ms(1000);   
	}	 
}
 


//    SIM800.sim.apn = "CMNET";
//    SIM800.sim.apn_user = "";
//    SIM800.sim.apn_pass = "";
//    SIM800.mqttServer.host = "mqtt-2.omnivoltaic.com";
//    SIM800.mqttServer.port = 1883;
//    SIM800.mqttClient.username = "Wisdon";
//    SIM800.mqttClient.pass = "123456";
//    SIM800.mqttClient.clientID = "TestSub";
//    SIM800.mqttClient.keepAliveInterval = 120;	

//==================================================================================
//
//
//
//
  u8 message_start[47]={//only for test
  0x10,0x2C,0x00,0x06,0x4D,0x51,0x49,0x73,0x64,0x70,0x03,0xC2,0x00,0x3C,0x00,
  0x06,0x41,0x42,0x43,0x44,0x45,0x46,0x00,0x08,0x64,0x78,0x78,0x6B,0x67,0x6B,
  0x70,0x70,0x00,0x0C,0x71,0x41,0x55,0x5A,0x42,0x64,0x61,0x53,0x49,0x55,0x4C,
  0x78,0x30
  };//0x13,0x00,0x08	
	
u8 MqttConnectData[57]={//only for test
	0x10, 0x37, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0xC2, 0x00, 0x78, 0x00, 
	0x0C, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x49, 0x64, 0x2D, 0x6B, 0x66, 0x62, 
	0x00, 0x0B, 0x35, 0x71, 0x75, 0x61, 0x6E, 0x37, 0x6B, 0x2F, 0x6B, 0x66, 0x62, 
  0x00, 0x10, 0x68, 0x50, 0x34, 0x55, 0x36, 0x78, 0x52, 0x62, 0x38, 0x78, 0x53, 
  0x52, 0x49, 0x61, 0x54, 0x5A
};
	

int MQTT_TASK2(void)
{
	int re=0;
	u8  rxbuf[20]={0};

	Omnivoltaic_GPRSConnctTcp("mqtt-2.omnivoltaic.com",1883);

	
	MQTT_Send_Packet_Test();
	
	while(1)
	{
      LED0=!LED0;//ÉÁË¸LED,ÌáÊ¾ÏµÍ³ÕýÔÚÔËÐÐ.
			delay_ms(1000);  	
	}	
	
/*
	  uint8_t sub = 0;
	
	  InitSim800();
	
    MQTT_Init();
	
	  MQTT_Pub("STM32", "test");
 
    while (0) 
		{
	
        if (SIM800.mqttServer.connect == 0) {
            MQTT_Init();
            sub = 0;
        }
        if (SIM800.mqttServer.connect == 1) {
            if(sub == 0){
                MQTT_Sub("test");
                sub = 1;
            }
            MQTT_Pub("STM32", "test");

            if(SIM800.mqttReceive.newEvent) {
                unsigned char *topic = SIM800.mqttReceive.topic;
                int payload = atoi(SIM800.mqttReceive.payload);
                SIM800.mqttReceive.newEvent = 0;
            }
        }
        delay_ms(1000);

    }	 	
	
	*/
	return 0;
}











//==================================================================================
//
//
//
//
int MQTT_TASK(void)
{
#if 0	
	   InitSim800();
	   MQTT_Init();
	   uint8_t sub = 0;
	 
    while (1) 
		{
        if (SIM800.mqttServer.connect == 0) {
            MQTT_Init();
            sub = 0;
        }
        if (SIM800.mqttServer.connect == 1) {
            if(sub == 0){
                //MQTT_Sub("test");
                sub = 1;
            }
						
						
            MQTT_Pub("STM32", "test");

            if(SIM800.mqttReceive.newEvent) {
                unsigned char *topic = SIM800.mqttReceive.topic;
               // int payload = atoi(SIM800.mqttReceive.payload);
                SIM800.mqttReceive.newEvent = 0;
            }
        }
        delay_ms(1000);   
				
        delay_ms(1000); 			
				
        delay_ms(1000); 				
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }	 	
	
#endif

	return 0;
}





#if 0



//????
#include "usart.h"
#include "delay.h"
 
//C?
#include <stdarg.h>
#include <string.h>
 
 
/*
************************************************************
*	????:	Usart1_Init
*
*	????:	??1???
*
*	????:	baud:??????
*
*	????:	?
*
*	??:		TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
 
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				//??GPIOA???
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);				//??USART1???
	
	//PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;					//???????
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;						//???Pin9
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;					//???????
	GPIO_Init(GPIOA, &gpioInitStruct);												//???GPIOA
	
	//PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;				//???????
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;						//???Pin10
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;					//???????
	GPIO_Init(GPIOA, &gpioInitStruct);												//???GPIOA
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//?????
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//?????
	usartInitStruct.USART_Parity = USART_Parity_No;					//???
	usartInitStruct.USART_StopBits = USART_StopBits_1;				//1????
	usartInitStruct.USART_WordLength = USART_WordLength_8b;				//8????
	USART_Init(USART1, &usartInitStruct);
	
	USART_Cmd(USART1, ENABLE);														//????
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//??????
	
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;					//usart1???
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;					//??????
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;				//???????(????????)
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;					//??????(????????)
	NVIC_Init(&nvicInitStruct);														//???NVIC
 
}
 
/*
************************************************************
*	????:	Usart2_Init
*
*	????:	??2???
*
*	????:	baud:??????
*
*	????:	?
*
*	??:		TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
 
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//?????
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//?????
	usartInitStruct.USART_Parity = USART_Parity_No;					//???
	usartInitStruct.USART_StopBits = USART_StopBits_1;				//1????
	usartInitStruct.USART_WordLength = USART_WordLength_8b;				//8????
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//????
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//??????
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
 
}
 
 
/*
************************************************************
*	????:	Usart3_Init
*
*	????:	??2???
*
*	????:	baud:??????
*
*	????:	?
*
*	??:		TX-PB10	RX-PB11
************************************************************
*/
void Usart3_Init(unsigned int baud)
{
 
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//USART1_TX   GPIOB.10
        gpioInitStruct.GPIO_Pin = GPIO_Pin_10; 				        //PB.10
        gpioInitStruct.GPIOSpeed = GPIO_Speed_50MHz;
        gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;			        //??????
        GPIO_Init(GPIOB, &gpioInitStruct);					//???GPIOB.10
   
        //USART1_RX	  GPIOB.11???
        gpioInitStruct.GPIO_Pin = GPIO_Pin_11;				        //PB.11
        gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		        //????
        GPIO_Init(GPIOB, &gpioInitStruct);					//???GPIOB.11 
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//?????
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				//?????
	usartInitStruct.USART_Parity = USART_Parity_No;						//???
	usartInitStruct.USART_StopBits = USART_StopBits_1;					//1????
	usartInitStruct.USART_WordLength = USART_WordLength_8b;					//8????
	USART_Init(USART3, &usartInitStruct);
	
	USART_Cmd(USART3, ENABLE);														//????
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);						//??????
	
	nvicInitStruct.NVIC_IRQChannel = USART3_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
 
}
 
/*
************************************************************
*	????:	Usart_SendString
*
*	????:	??????
*
*	????:	USARTx:???
*				str:??????
*				len:????
*
*	????:	?
*
*	??:		
************************************************************
*/
 
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
 
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);						//????
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//??????
	}
 
}
 
/*
************************************************************
*	????:	UsartPrintf
*
*	????:	?????
*
*	????:	USARTx:???
*				fmt:????
*
*	????:	?
*
*	??:		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
 
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);				//???
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}
 
}
 
extern unsigned char usart1Buf[64];
extern unsigned char usart1Len;
/*
************************************************************
*	????:	USART1_IRQHandler
*
*	????:	??1????
*
*	????:	?
*
*	????:	?
*
*	??:		
************************************************************
*/
void USART1_IRQHandler(void)
{
 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//????
	{
		if(usart1Len >= 64)									//??????,??????
			usart1Len = 0;
		usart1Buf[usart1Len++] = USART1->DR;
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
	}
 
}
 
extern char usart2Buf[64];
extern char usart2Len;
/*
************************************************************
*	????:	USART2_IRQHandler
*
*	????:	??2????
*
*	????:	?
*
*	????:	?
*
*	??:		
************************************************************
*/
void USART2_IRQHandler(void)
{
 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	//????
	{
		if(usart2Len >= 64)									//??????,??????
			usart2Len = 0;
		usart2Buf[usart2Len++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
 
 
 
extern char usart3Buf[64];
extern char usart3Len;
/*
************************************************************
*	????:	USART3_IRQHandler
*
*	????:	??3????
*
*	????:	?
*
*	????:	?
*
*	??:		
************************************************************
*/
void USART3_IRQHandler(void)
{
 
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)	//????
	{
		if(usart3Len >= 64)									//??????,??????
			usart3Len = 0;
		usart3Buf[usart3Len++] = USART3->DR;
		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}
}







#endif






