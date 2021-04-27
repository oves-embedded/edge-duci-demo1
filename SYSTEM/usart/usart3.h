#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 

#define USART3_REC_LEN  			1024  	//定义最大接收字节数 200
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
//extern u8  Uart3_Rxbuf[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u16 USART3_RX_STA;         		//接收状态标记	

//extern u16 uart3rcvit_cnt;//
//如果想串口中断接收，请不要注释以下宏定义
//void Uart3_Init(u32 bound);
    // Uart3_Init
//void Uart3_Init2(u32 baud);


typedef struct _UART_FUNCTION{
   // u16 _rx_num;
   // u16 _rcvit_cnt;	//reserved
	 // u16 _rx_buff_strat;
	  void (*InitUart)(u32 baud);  
	  int  (*SendData)(u8* buf,u16 buflen,u16 timeout ); 
	  int  (*Receive )(u8* buf,u16 len   ,u16 timeout );
	
	  u16 _RxNum;		// receive cnt
	  u16 _RxHead;	
	  u16 _RxTail;	
	  u16 (*GetHeadByte)(void);
	  void(*AddTailByte)(u8 ch);	
	  void(*ClearRxBuff)(void);

    u8  _Rxbuf[USART3_REC_LEN];	
	
}UART_FUNCTION;

extern UART_FUNCTION  Uart3;
void testloop(void);

void USART_DEBUG_SendStr1(char * str);

int  Uart1_SendData2(u8* buf,u16 buflen,u16 timeout );
int  SendDataToSim800c(u8* buf,u16 buflen,u16 timeout);
int  FindStrFormSim800cResponds(char* str,int clr_rxbuf,u16 waitimes);


//void Init2(u32 baud);
//int  Uart3_SendData(u8* buf,u16 buflen,u16 timeout );
//int  Uart3_Receive (u8* buf,u16 len   ,u16 timeout );


#endif


